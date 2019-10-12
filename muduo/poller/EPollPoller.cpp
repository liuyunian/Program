#include <unistd.h>     // close
#include <string.h>     // memset
#include <assert.h>     // assert
#include <poll.h>
#include <sys/epoll.h>

#include <tools/log/log.h>
#include <tools/base/Timestamp.h>

#include "EPollPoller.h"

// On Linux, the constants of poll(2) and epoll(4)
// are expected to be the same.
static_assert(EPOLLIN == POLLIN,        "epoll uses same flag values as poll");
static_assert(EPOLLPRI == POLLPRI,      "epoll uses same flag values as poll");
static_assert(EPOLLOUT == POLLOUT,      "epoll uses same flag values as poll");
static_assert(EPOLLRDHUP == POLLRDHUP,  "epoll uses same flag values as poll");
static_assert(EPOLLERR == POLLERR,      "epoll uses same flag values as poll");
static_assert(EPOLLHUP == POLLHUP,      "epoll uses same flag values as poll");

const int k_new = -1;       // 新的Channel
const int k_added = 1;      // 已经添加过的Channel
const int k_deleted = 2;    // 已经标记删除的Channel

const int EPollPoller::k_initEventListSize = 16;

EPollPoller::EPollPoller(EventLoop* loop) : 
    Poller(loop),
    m_epfd(::epoll_create1(EPOLL_CLOEXEC)),
    m_eventList(k_initEventListSize)
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

        fill_activeChannels(numEvents, activeChannels);
        if(numEvents == m_eventList.size()){                // 需要扩容
            m_eventList.resize(m_eventList.size() * 2);
        }
    }

    return now;
}

void EPollPoller::fill_activeChannels(int numEvents, ChannelList* activeChannels) const {
    assert(numEvents <= m_eventList.size());

    Channel* channel;
    std::map<int, Channel*>::const_iterator iter_ch;
    for(int i = 0; i < numEvents; ++ i){
        channel = static_cast<Channel*>(m_eventList[i].data.ptr);
        iter_ch = m_channelStore.find(channel->get_fd());
        assert(iter_ch != m_channelStore.end());
        assert(iter_ch->second == channel);

        channel->set_revents(m_eventList[i].events);
        activeChannels->push_back(channel);
    }
}

void EPollPoller::update_channel(Channel * channel){
    assert_in_loopThread();                                                     // 从父类Poller继承来的成员函数

    const int index = channel->get_index();
    int channel_fd = channel->get_fd();
    LOG_DEBUG("fd = %d events = %d index = %d", channel_fd, channel->get_events(), index);

    if(index == k_new || index == k_deleted){
        if(index == k_new){                                                     // 新Channel
            assert(m_channelStore.find(channel_fd) == m_channelStore.end());
            m_channelStore.insert({channel_fd, channel});
        }
        else{                                                                   // 之前标记过已经删除的Channel
            assert(m_channelStore.find(channel_fd) != m_channelStore.end());
            assert(m_channelStore[channel_fd] == channel);
        }

        channel->set_index(k_added);                                            // 标记该通道是已经添加过的了
        update(EPOLL_CTL_ADD, channel);
    }
    else{
        assert(m_channelStore.find(channel_fd) != m_channelStore.end());
        assert(m_channelStore[channel_fd] == channel);
        assert(index == k_added);                                               // 断言当前的channel是已经添加过的

        if(channel->is_noneEvents()){                                           // Channel关注的事件为空
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(k_deleted);
        }
        else{
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::remove_channel(Channel * channel){
    assert_in_loopThread();

    int index = channel->get_index();
    int channel_fd = channel->get_fd();
    LOG_DEBUG("fd = %d events = %d index = %d", channel_fd, channel->get_events(), index);

    assert(m_channelStore.find(channel_fd) != m_channelStore.end());
    assert(m_channelStore[channel_fd] == channel);
    assert(channel->is_noneEvents());
    assert(index == k_added || index == k_deleted);

    size_t n = m_channelStore.erase(channel_fd);
    assert(n == 1);                                                         // 断言删除的元素个数为1

    if(index == k_added){
        update(EPOLL_CTL_DEL, channel);
    }

    channel->set_index(k_new);
}

void EPollPoller::update(int operation, Channel * channel){
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = channel->get_events();
    event.data.ptr = channel;
    int channel_fd = channel->get_fd();

    if(::epoll_ctl(m_epfd, operation, channel_fd, &event) < 0){
        if(operation == EPOLL_CTL_DEL){
            LOG_SYSERR("epoll_ctl op = %d fd = %d", operation, channel_fd);
        }
        else{
            LOG_SYSFATAL("epoll_ctl op = %d fd = %d", operation, channel_fd);
        }
    }
}