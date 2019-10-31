#include <algorithm>    // iter_swap
#include <assert.h>     // assert
#include <poll.h>

#include <tools/log/log.h>

#include "muduo/poller/PollPoller.h"
#include "muduo/Channel.h"

PollPoller::PollPoller(EventLoop* loop)
  : Poller(loop){}

PollPoller::~PollPoller(){}

Timestamp PollPoller::poll(int timeoutMs, std::vector<Channel*>* activeChannels){
    int numEvents = ::poll(m_pollfdList.data(), m_pollfdList.size(), timeoutMs);
    int savedErrno = errno;
    Timestamp now(Timestamp::now());
    if(numEvents < 0){
        errno = savedErrno;
        LOG_SYSERR("PollPoller::poll()");
    }
    else if(numEvents == 0){
        LOG_DEBUG("nothing happended");
    }
    else{
        LOG_DEBUG("%d events happended", numEvents);
        fill_active_channels(numEvents, activeChannels);
    }

    return now;
}

void PollPoller::fill_active_channels(int numEvents, std::vector<Channel*>* activeChannels) const {
    std::map<int, Channel*>::const_iterator iter;
    Channel* channel;

    for(auto pollfd = m_pollfdList.begin(); pollfd != m_pollfdList.end() && numEvents > 0; ++ pollfd){
        if(pollfd->revents > 0){
            -- numEvents;

            iter = m_channelStore.find(pollfd->fd);
            assert(iter != m_channelStore.end());
            channel = iter->second;
            assert(channel->get_fd() == pollfd->fd);
            channel->set_revents(pollfd->revents);
            activeChannels->push_back(channel);
        }
    }
}

void PollPoller::update_channel(Channel* channel){
    m_ownerLoop->assert_in_loop_thread();

    int fd = channel->get_fd();
    LOG_DEBUG("fd = %d events = %d", fd, channel->get_events());

    // 在poll模式中index用来记录在pollfdList中的位置
    // index < 0表示是一个新Channel，也就是说这里的操作是添加
    if(channel->get_index() < 0){                                           
        assert(m_channelStore.find(fd) == m_channelStore.end());
        m_channelStore.insert({fd, channel});

        struct pollfd pfd;
        pfd.fd = fd;
        pfd.events = static_cast<short>(channel->get_events());
        pfd.revents = 0;
        m_pollfdList.push_back(pfd);

        channel->set_index(m_pollfdList.size() - 1);
    }
    else{
        assert(m_channelStore.find(fd) != m_channelStore.end());
        assert(m_channelStore[fd] == channel);

        int index = channel->get_index();
        assert(index >= 0 && index < m_pollfdList.size());

        struct pollfd& pfd = m_pollfdList[index];
        // 注意到pfd.fd有两种取值：channel.fd或者channel.fd相反数减1
        // channel.fd一定是正数，所以其相反数减一必然是负数，置为负数表示某个Channel暂时不关心任何事件
        assert(pfd.fd == channel->get_fd() || pfd.fd == -channel->get_fd() - 1);
        pfd.events = static_cast<short>(channel->get_events());
        pfd.revents = 0;

        if(channel->is_none_events()){
            pfd.fd = -channel->get_fd()-1;
        }
    }
}

void PollPoller::remove_channel(Channel* channel){
    m_ownerLoop->assert_in_loop_thread();

    int fd = channel->get_fd();
    LOG_DEBUG("fd = %d events = %d", fd, channel->get_events());
    
    assert(m_channelStore.find(fd) != m_channelStore.end());
    assert(m_channelStore[fd] == channel);
    // assert(channel->is_none_events());                               // Channel::remove()中已经断言过了

    int index = channel->get_index();
    assert(index >= 0 && index < m_pollfdList.size());

    struct pollfd & pfd = m_pollfdList[index];
    assert(pfd.fd == -channel->get_fd() - 1);
    assert(pfd.events == channel->get_events());

    size_t n = m_channelStore.erase(fd);
    assert(n == 1);

    if(index == m_pollfdList.size() - 1){                               // m_pollfdList中的最后一个元素
        m_pollfdList.pop_back();
    }
    else{                                                               // 移除元素的时间复杂度为O(1)
        int channelAtEnd = m_pollfdList.back().fd;
        iter_swap(m_pollfdList.begin() + index, m_pollfdList.end()-1);  // 当前要移除的元素迭代器与最后一个元素迭代器交换
        if(channelAtEnd < 0){
            channelAtEnd = -channelAtEnd - 1;
        }
        m_channelStore[channelAtEnd]->set_index(index);
        m_pollfdList.pop_back();
    }
}
