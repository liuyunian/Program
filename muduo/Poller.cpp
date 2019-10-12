#include <stdlib.h> // getenv

#include "Poller.h"
#include "poller/PollPoller.h"
#include "poller/EPollPoller.h"

Poller::Poller(EventLoop * loop) : 
    m_ownerLoop(loop){}

bool Poller::has_channel(Channel* channel) const {
    assert_in_loopThread();
    auto iter = m_channelStore.find(channel->get_fd());
    return iter != m_channelStore.end() && iter->second == channel;
}

Poller* Poller::new_default_Poller(EventLoop * loop){ // 默认采用的IO复用方式是epoll
    if(::getenv("USE_POLL")){
        return new PollPoller(loop);
    }
    else{                              
        return new EPollPoller(loop);
    }
}