#include <poll.h>           // poll
#include <assert.h>         // assert
#include <tools_cxx/log.h>

#include "EventLoop.h"

__thread EventLoop * t_loopInThisThread = nullptr;

EventLoop::EventLoop() : 
    m_looping(false),
    m_threadID(CurrentThread::get_tid())
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

    poll(NULL, 0, 5000); // 等待5秒
    LOG_INFO("EventLoop %p stop looping", this);
    m_looping = false;
}

void EventLoop::abort_not_in_loopThread(){
    LOG_FATAL("EventLoop %p was created in %d thread, but current thread is %d", this, m_threadID, CurrentThread::get_tid());
}