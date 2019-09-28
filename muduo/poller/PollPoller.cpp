#include <algorithm>    // iter_swap
#include <assert.h>     // assert
#include <tools_cxx/log.h>

#include "PollPoller.h"
#include "../Channel.h"

PollPoller::PollPoller(EventLoop* loop)
  : Poller(loop){}

PollPoller::~PollPoller(){}

Timestamp PollPoller::poll(int timeoutMs, std::vector<Channel *> * activeChannels){
    int numEvents = ::poll(m_pollfdList.data(), m_pollfdList.size(), timeoutMs);    // 注意这里要采用全局的poll()
    Timestamp now(Timestamp::now());
    if(numEvents < 0){
        LOG_SYSERR("PollPoller::poll()");
    }
    else if(numEvents == 0){
        LOG_INFO("nothing happended");
    }
    else{
        // LOG_INFO("%d events happended", numEvents);
        fill_activeChannels(numEvents, activeChannels);
    }

    return now;
}

void PollPoller::fill_activeChannels(int numEvents, std::vector<Channel *> * activeChannels) const {
    std::map<int, Channel *>::const_iterator iter_ch;
    Channel * channel;

    for(auto & pollfd : m_pollfdList){
        if(pollfd.revents > 0){ // 表示有事件发生
            -- numEvents;

            iter_ch = m_channelStore.find(pollfd.fd);
            assert(iter_ch != m_channelStore.end());
            channel = iter_ch->second;
            assert(channel->get_fd() == pollfd.fd);
            channel->set_revents(pollfd.revents);
            activeChannels->push_back(channel);
        }
    }
}

void PollPoller::update_channel(Channel * channel){
    assert_in_loopThread();                                                 // 从父类Poller继承来的成员函数

    int channel_fd = channel->get_fd();
    // LOG_INFO("fd = %d events = %d", channel_fd, channel->get_events());
    if(channel->get_index() < 0){                                           // index < 0表示是一个新通道，也就是说这里的操作是添加而不是修改
        assert(m_channelStore.find(channel_fd) == m_channelStore.end());    // 断言在m_channelStore中确实没有该channel
        m_channelStore.insert({channel_fd, channel});

        struct pollfd pfd;
        pfd.fd = channel_fd;
        pfd.events = static_cast<short>(channel->get_events());
        pfd.revents = 0;
        m_pollfdList.push_back(pfd);

        channel->set_index(m_pollfdList.size() - 1);
    }
    else{                                                                   // 修改操作
        assert(m_channelStore.find(channel_fd) != m_channelStore.end());    // 断言key = channel_fd在m_channelStore中存在
        assert(m_channelStore[channel_fd] == channel);                      // 断言对应的value = 参数channel

        int index = channel->get_index();
        assert(index >= 0 && index < m_pollfdList.size());                  // 断言该channel对应在m_pollfdList位置是合理的

        struct pollfd & pfd = m_pollfdList[index];
        assert(pfd.fd == channel->get_fd() || pfd.fd == -channel->get_fd() - 1); // 负号的优先级要高于减号，fd = 3时，-channel->get_fd()-1为-4， 为什么要有这样的操作
        pfd.events = static_cast<short>(channel->get_events());
        pfd.revents = 0;

        if(channel->is_noneEvents()){                                       // 不再关注任何事件
            pfd.fd = -channel->get_fd()-1;                                  // 为了remove_channel优化
        }
    }
}

void PollPoller::remove_channel(Channel * channel){
    assert_in_loopThread();                                             // 从父类Poller继承来的成员函数

    int channel_fd = channel->get_fd();
    // LOG_INFO("fd = %d events = %d", channel_fd, channel->get_events());
    
    assert(m_channelStore.find(channel_fd) != m_channelStore.end());    // 断言key = channel_fd在m_channelStore中存在
    assert(m_channelStore[channel_fd] == channel);                      // 断言对应的value = 参数channel
    assert(channel->is_noneEvents());                                   // 断言channel没有关注的事件

    int index = channel->get_index();
    assert(index >= 0 && index < m_pollfdList.size());                  // 断言该channel对应在m_pollfdList位置是合理的

    struct pollfd & pfd = m_pollfdList[index];
    assert(pfd.fd == -channel->get_fd() - 1);
    assert(pfd.events == channel->get_events());

    size_t n = m_channelStore.erase(channel_fd);                        // n表示移除元素的个数
    assert(n == 1);

    if(index == m_pollfdList.size() - 1){                               // m_pollfdList中的最后一个元素
        m_pollfdList.pop_back();
    }
    else{                                                               // 移除元素的时间复杂度为O(1)
        int channelAtEnd = m_pollfdList.back().fd;

        iter_swap(m_pollfdList.begin() + index, m_pollfdList.end() - 1); // 当前要移除的元素迭代器与最后一个元素迭代器交换
        if(channelAtEnd < 0){
            channelAtEnd = -channelAtEnd - 1;
        }
        m_channelStore[channelAtEnd]->set_index(index);

        m_pollfdList.pop_back();
    }
}
