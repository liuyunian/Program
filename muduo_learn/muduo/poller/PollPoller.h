#ifndef POLLPOLLER_H_
#define POLLPOLLER_H_

#include <vector>

#include "muduo/Poller.h"

// 前向声明
struct pollfd;
class Channel;

// IO复用 -- poll
class PollPoller : public Poller{
public:
    PollPoller(EventLoop* loop);

    virtual ~PollPoller() override;

    // 实现从父类继承的纯虚函数
    virtual Timestamp poll(int timeoutMs, std::vector<Channel*>* activeChannels) override;
    virtual void update_channel(Channel * channel) override;
    virtual void remove_channel(Channel * channel) override;

private:
    void fill_active_channels(int numEvents, std::vector<Channel*>* activeChannels) const ; 

private:
    std::vector<struct pollfd> m_pollfdList;
};

#endif // POLLPOLLER_H_