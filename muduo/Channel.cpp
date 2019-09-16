#include "Channel.h"

const int k_noneEvent = 0;
const int k_readEvent = POLL_IN | POLL_PRI;
const int k_writeEvent = POLL_OUT;

Channel::Channel(EventLoop * loop, int fd) : 
    m_loop(loop),
    m_fd(fd),
    m_events(0),
    m_revents(0),
    m_index(-1)
{

}

Channel::~Channel(){

}

void Channel::handleEvent(Timestamp receiveTime){
    
}

