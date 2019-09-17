#include <stdlib.h> // getenv

#include "Poller.h"
#include "poller/PollPoller.h"
#include "poller/EPollPoller.h"

Poller::Poller(EventLoop * loop) : 
    m_ownerLoop(loop){}

Poller * Poller::new_default_Poller(EventLoop * loop){
    if(::getenv("USE_POLL")){
        return new PollPoller(loop);
    }
    else{
        return new EPollPoller(loop);
    }
}