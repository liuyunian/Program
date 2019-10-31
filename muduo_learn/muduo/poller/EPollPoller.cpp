#include <unistd.h>     // close
#include <string.h>     // memset
#include <assert.h>     // assert
#include <poll.h>
#include <sys/epoll.h>

#include <tools/log/log.h>
#include <tools/base/Timestamp.h>

#include "muduo/poller/EPollPoller.h"
#include "muduo/Channel.h"

// On Linux, the constants of poll(2) and epoll(4) are expected to be the same.
static_assert(EPOLLIN == POLLIN,        "epoll uses same flag values as poll");
static_assert(EPOLLPRI == POLLPRI,      "epoll uses same flag values as poll");
static_assert(EPOLLOUT == POLLOUT,      "epoll uses same flag values as poll");
static_assert(EPOLLRDHUP == POLLRDHUP,  "epoll uses same flag values as poll");
static_assert(EPOLLERR == POLLERR,      "epoll uses same flag values as poll");
static_assert(EPOLLHUP == POLLHUP,      "epoll uses same flag values as poll");

const int kNew = -1;       // 新的Channel
const int kAdded = 1;      // 已经添加过的Channel
const int kDeleted = 2;    // 已经标记删除的Channel

const int EPollPoller::m_kInitEventListSize = 16;

EPollPoller::EPollPoller(EventLoop* loop) : 
    Poller(loop),
    m_epfd(::epoll_create1(EPOLL_CLOEXEC)),
    m_eventList(m_kInitEventListSize)
{
    if(m_epfd < 0){
        LOG_SYSFATAL("Failed to created epfd in EPollPoller::EPollPoller(EventLoop*)");
    }
}

EPollPoller::~EPollPoller(){
    close(m_epfd);
}

Timestamp EPollPoller::poll(int timeoutMs, ChannelList* activeChannels){
    int numEvents = ::epoll_wait(m_epfd, m_eventList.data(), m_eventList.size(), timeoutMs);

    int savedErrno = errno;
    Timestamp now(Timestamp::now());                        // 可能会改变errno

    if(numEvents < 0){
        if(savedErrno != EINTR){                            // EINTR错误不用报错
            errno = savedErrno;
            LOG_SYSERR("EPollPoller::poll()");
        }
    }
    else if(numEvents == 0){
        LOG_DEBUG("nothing happended");
    }
    else{
        LOG_DEBUG("%d events happended", numEvents);
        fill_active_channels(numEvents, activeChannels);
        if(numEvents == m_eventList.size()){                // 需要扩容
            m_eventList.resize(m_eventList.size() * 2);
        }
    }

    return now;
}

void EPollPoller::fill_active_channels(int numEvents, ChannelList* activeChannels) const {
    assert(numEvents <= m_eventList.size());

    Channel* channel;
    std::map<int, Channel*>::const_iterator iter;
    for(int i = 0; i < numEvents; ++ i){
        channel = static_cast<Channel*>(m_eventList[i].data.ptr);
        iter = m_channelStore.find(channel->get_fd());
        assert(iter != m_channelStore.end());
        assert(iter->second == channel);

        channel->set_revents(m_eventList[i].events);
        activeChannels->push_back(channel);
    }
}

void EPollPoller::update_channel(Channel* channel){
    m_ownerLoop->assert_in_loop_thread();

    /**
     * 在epoll模式中，Channel的index用来区别该Channel对象EPollPoller的状态
     * kNew -- 没有添加关注过
     * kAdded -- 已经添加关注过，此时updata_channel表示修改关注的事件，remove_channel表示移除不再关注事件
     * kDeleted -- 以及被移除，可以再次被添加
    */ 
    const int index = channel->get_index();
    int fd = channel->get_fd();
    LOG_DEBUG("fd = %d events = %d index = %d", fd, channel->get_events(), index);
    if(index == kNew || index == kDeleted){
        if(index == kNew){                                                                  // 新Channel
            assert(m_channelStore.find(fd) == m_channelStore.end());
            m_channelStore.insert({fd, channel});
        }
        else{                                                                               // 之前标记过已经删除的Channel
            assert(m_channelStore.find(fd) != m_channelStore.end());
            assert(m_channelStore[fd] == channel);
        }

        channel->set_index(kAdded);                                                         // 标记该Channel对象是已经添加过的了
        update(EPOLL_CTL_ADD, channel);
    }
    else{                                                                                   // 该Channel之前已经添加过了，现在的操作是修改关注的事件
        assert(m_channelStore.find(fd) != m_channelStore.end());
        assert(m_channelStore[fd] == channel);
        assert(index == kAdded);

        if(channel->is_none_events()){                                                      // Channel关注的事件为空，此时移除该Channel
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else{
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::remove_channel(Channel* channel){
    m_ownerLoop->assert_in_loop_thread();

    int index = channel->get_index();
    int fd = channel->get_fd();
    LOG_DEBUG("fd = %d events = %d index = %d", fd, channel->get_events(), index);

    assert(m_channelStore.find(fd) != m_channelStore.end());
    assert(m_channelStore[fd] == channel);
    // assert(channel->is_none_events());                                           // 在Channell::remove中已经断言过了
    assert(index == kAdded || index == kDeleted);                                   // 什么情况下存在标记为kDeleted，但是没有从m_channelStore中移除？

    size_t n = m_channelStore.erase(fd);
    assert(n == 1);

    if(index == kAdded){
        update(EPOLL_CTL_DEL, channel);
    }

    channel->set_index(kNew);
}

void EPollPoller::update(int operation, Channel* channel){
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = channel->get_events();
    event.data.ptr = channel;
    int fd = channel->get_fd();
    LOG_DEBUG("epoll_ctl fd = %d, operation = %s, event = %s", fd, operation_to_string(operation), channel->events_to_string().c_str());
    if(::epoll_ctl(m_epfd, operation, fd, &event) < 0){
        if(operation == EPOLL_CTL_DEL){
            LOG_SYSERR("epoll_ctl op = %s fd = %d", operation_to_string(operation), fd);
        }
        else{
            LOG_SYSFATAL("epoll_ctl op = %s fd = %d", operation_to_string(operation), fd);
        }
    }
}

const char* EPollPoller::operation_to_string(int operation){
    switch(operation){
    case EPOLL_CTL_ADD:
        return "ADD";
    case EPOLL_CTL_DEL:
        return "DEL";
    case EPOLL_CTL_MOD:
        return "MOD";
    default:
        assert(false && "ERROR op");
        return "Unknown Operation";
    }
}