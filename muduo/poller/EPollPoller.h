#ifndef EPOLLPOLLER_H_
#define EPOLLPOLLER_H_

#include "../Poller.h"
#include "../EventLoop.h"

// IO复用 -- epoll
class EPollPoller : public Poller{
public:
    EPollPoller(EventLoop * loop);

    virtual ~EPollPoller();
};

#endif // EPOLLPOLLER_H_