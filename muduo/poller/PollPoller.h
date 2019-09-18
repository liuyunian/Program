#ifndef POLLPOLLER_H_
#define POLLPOLLER_H_

#include <vector>
#include <map>

#include <poll.h>
#include <tools_cxx/Timestamp.h>

#include "../Poller.h"


// IO复用 -- poll
class PollPoller : public Poller{
public:
    PollPoller(EventLoop * loop);

    virtual ~PollPoller();

    // 实现从父类继承的纯虚函数
    virtual Timestamp poll(int timeoutMs, std::vector<Channel *> * activeChannels);
    virtual void update_channel(Channel * channel);
    virtual void remove_channel(Channel * channel);

private:
    void fill_activeChannels(int numEvents, std::vector<Channel *> * activeChannels) const ; 

private:
    std::vector<struct pollfd> m_pollfdList;
    std::map<int, Channel *> m_channelStore; // key: 文件描述符， value: Channel *
};

#endif // POLLPOLLER_H_