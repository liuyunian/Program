#ifndef EPOLLPOLLER_H_
#define EPOLLPOLLER_H_

#include <vector>
#include <map>

#include "../Poller.h"
#include "../EventLoop.h"

// IO复用 -- epoll
class EPollPoller : public Poller{
public:
    EPollPoller(EventLoop * loop);

    virtual ~EPollPoller();

    // 实现从父类继承的纯虚函数
    virtual Timestamp poll(int timeoutMs, std::vector<Channel *> * activeChannels);
    virtual void update_channel(Channel * channel);
    virtual void remove_channel(Channel * channel);

private:
    void fill_activeChannels(int numEvents, std::vector<Channel *> * activeChannels) const;
    void update(int operation, Channel * channel);

private:
    static const int k_initEventListSize;

    int m_epfd;
    std::vector<struct epoll_event> m_eventList;
    std::map<int, Channel *> m_channelStore;        // key: 文件描述符， value: Channel *
};

#endif // EPOLLPOLLER_H_