#include <stdlib.h> // getenv

#include "muduo/Poller.h"
#include "muduo/Channel.h"
#include "muduo/poller/PollPoller.h"
#include "muduo/poller/EPollPoller.h"

Poller::Poller(EventLoop * loop) : 
    m_ownerLoop(loop){}

bool Poller::has_channel(Channel* channel) const {
    m_ownerLoop->assert_in_loop_thread();
    auto iter = m_channelStore.find(channel->get_fd());
    return iter != m_channelStore.end() && iter->second == channel;
}

Poller* Poller::new_default_Poller(EventLoop * loop){
    if(::getenv("USE_POLL")){
        return new PollPoller(loop);
    }
    else{                              
        return new EPollPoller(loop);
    }
}