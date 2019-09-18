#include <algorithm>

#include <poll.h>           // poll
#include <assert.h>         // assert
#include <tools_cxx/log.h>

#include "EventLoop.h"
#include "Channel.h"
#include "Poller.h"

__thread EventLoop * t_loopInThisThread = nullptr;

const int k_pollTimeoutMs = 10000; // 10秒

EventLoop::EventLoop() : 
    m_looping(false),
    m_threadID(CurrentThread::get_tid()),
    m_poller(Poller::new_default_Poller(this))
{
    LOG_INFO("EventLoop %p created in thread %d", this, m_threadID);
 
    if(t_loopInThisThread != nullptr){
        LOG_FATAL("Another EventLoop exists in this thread %d", m_threadID);
    }
    else{
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop(){
    assert(!m_looping); // 断言不处于事件循环中
    t_loopInThisThread = nullptr;
}

void EventLoop::loop(){ // 该成员函数只能在创建EventLoop对象的线程中调用，不能跨线程调用
    assert(!m_looping);
    assert_in_loopThread();

    m_looping = true;

    // poll(NULL, 0, 5000); // 等待5秒
    while(!m_quit){
        m_activeChannels.clear();

        m_pollReturnTime = m_poller->poll(k_pollTimeoutMs, &m_activeChannels);
        m_eventHandling = true;

        for(auto & ch : m_activeChannels){
            m_currentActiveChannel = ch;
            m_currentActiveChannel->handleEvent(m_pollReturnTime);
        }
        
        m_currentActiveChannel = nullptr;
        m_eventHandling = false;
    }

    LOG_INFO("EventLoop %p stop looping", this);
    m_looping = false;
}

void EventLoop::quit(){
    m_quit = true;
    if(!is_in_loopThread()){

    }
}

void EventLoop::update_channel(Channel * channel){
    assert(channel->get_ownerLoop() == this);
    assert_in_loopThread();

    m_poller->update_channel(channel);
}

void EventLoop::remove_channel(Channel * channel){
    assert(channel->get_ownerLoop() == this);
    assert_in_loopThread();

    if(m_eventHandling){
        assert(m_currentActiveChannel == channel || std::find(m_activeChannels.begin(), m_activeChannels.end(), channel) == m_activeChannels.end());
    }

    m_poller->remove_channel(channel);
}

void EventLoop::abort_not_in_loopThread(){
    LOG_FATAL("EventLoop %p was created in %d thread, but current thread is %d", this, m_threadID, CurrentThread::get_tid());
}