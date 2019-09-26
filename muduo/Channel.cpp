#include <poll.h>           // poll POLLIN ...
#include <assert.h>         // assert
#include <tools_cxx/log.h>

#include "Channel.h"

const int Channel::k_noneEvent = 0;
const int Channel::k_readEvent = POLLIN | POLLPRI;
const int Channel::k_writeEvent = POLLOUT;

Channel::Channel(EventLoop * loop, int fd) : 
    m_loop(loop),
    m_fd(fd),
    m_events(0),
    m_revents(0),
    m_index(-1),
    m_tied(false),
    m_eventHandling(false)
{

}

Channel::~Channel(){
    assert(!m_eventHandling);
}

void Channel::handleEvent(Timestamp receiveTime){
    std::shared_ptr<void> guard;
    if(m_tied){                                     // 该Channel对象已经绑定
        guard = m_tie.lock();                       // 获取std::weak_ptr监视的shared_ptr
        
        if(guard){
            handleEvent_with_guard(receiveTime);
        }
    }
    else{
        // ... 
        handleEvent_with_guard(receiveTime);
    }
}

void Channel::tie(const std::shared_ptr<void> & obj){
    m_tie = obj;
    m_tied = true;
}

void Channel::handleEvent_with_guard(Timestamp receiveTime){
    m_eventHandling = true;

    if((m_revents & POLLHUP) && !(m_revents & POLLIN)){ // POLLHUP事件并且不是POLLIN事件
        if(m_logHup){
            LOG_WARN("Channel::handle_event() POLLHUP");
        }

        if(m_closeCallback){
            m_closeCallback();
        }
    }

    if(m_revents & POLLNVAL){                           // POLLNVAL事件：文件描述符不是一个打开的文件
        LOG_WARN("Channel::handle_event() POLLNVAL");
    }

    if(m_revents & (POLLERR | POLLNVAL)){               // POLLERR事件或者POLLNVAL事件
        if(m_errorCallback){
            m_errorCallback();
        }
    }

    if(m_revents & (POLLIN | POLLPRI | POLLRDHUP)){      // 读事件 或者 POLLPRI高优先级可读事件 | POLLRDHUP连接对方关闭连接
        if(m_readCallback){
            m_readCallback(receiveTime);
        }
    }

    if(m_revents & POLLOUT){
        if(m_writeCallback){
            m_writeCallback();
        }
    }

    m_eventHandling = false;
}

void Channel::update(){
    m_loop->update_channel(this);
}

void Channel::remove(){
    assert(is_noneEvents()); // 断言此时关注的事件为空
    m_loop->remove_channel(this);
}

