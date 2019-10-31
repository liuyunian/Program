#ifndef EPOLLPOLLER_H_
#define EPOLLPOLLER_H_

#include <vector>

#include "muduo/Poller.h"

// 前向声明
struct epoll_event;
class Channel;

// IO复用 -- epoll
class EPollPoller : public Poller {
public:
    EPollPoller(EventLoop* loop);

    virtual ~EPollPoller() override;

    // 实现从父类继承的纯虚函数
    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;
    virtual void update_channel(Channel* channel) override;
    virtual void remove_channel(Channel* channel) override;

private:
    void fill_active_channels(int numEvents, ChannelList* activeChannels) const;
    void update(int operation, Channel* channel);

    // for debug
    const char* operation_to_string(int operation);

private:
    static const int m_kInitEventListSize;  // epoll返回的事件列表的初始长度

    int m_epfd;
    std::vector<struct epoll_event> m_eventList;
};

#endif // EPOLLPOLLER_H_