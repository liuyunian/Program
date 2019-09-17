#include "EPollPoller.h"

EPollPoller::EPollPoller(EventLoop * loop) : 
    Poller(loop){}

EPollPoller::~EPollPoller(){}