#ifndef POLLPOLLER_H_
#define POLLPOLLER_H_

#include "../Poller.h"
#include "../EventLoop.h"

// IO复用 -- poll
class PollPoller : public Poller{
public:
    PollPoller(EventLoop * loop);

    virtual ~PollPoller();

private:

};

#endif // POLLPOLLER_H_