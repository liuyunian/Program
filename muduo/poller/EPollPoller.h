#ifndef EPOLLPOLLER_H_
#define EPOLLPOLLER_H_

#include <vector>

#include "Poller.h"
#include "EventLoop.h"

// IO复用 -- epoll
class EPollPoller : public Poller {
public:
    EPollPoller(EventLoop * loop);

    virtual ~EPollPoller() override;

    // 实现从父类继承的纯虚函数
    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;
    virtual void update_channel(Channel* channel) override;
    virtual void remove_channel(Channel* channel) override;

private:
    void fill_activeChannels(int numEvents, ChannelList* activeChannels) const;
    void update(int operation, Channel* channel);

private:
    static const int k_initEventListSize;

    int m_epfd;
    std::vector<struct epoll_event> m_eventList;
};

#endif // EPOLLPOLLER_H_