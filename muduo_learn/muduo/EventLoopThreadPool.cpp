#include <assert.h>

#include "muduo/EventLoopThreadPool.h"
#include "muduo/EventLoop.h"
#include "muduo/EventLoopThread.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop) : 
    m_started(false),
    m_numThreads(0),
    m_next(0),
    m_baseLoop(baseLoop){}

EventLoopThreadPool::~EventLoopThreadPool(){}

void EventLoopThreadPool::start(){
    assert(!m_started);
    m_baseLoop->assert_in_loop_thread();
    m_started = true;

    for(int i = 0; i < m_numThreads; ++ i){
        EventLoopThread* thread = new EventLoopThread();
        m_loops.push_back(thread->start_loop());
        m_threads.push_back(std::unique_ptr<EventLoopThread>(thread));
    }
}

EventLoop* EventLoopThreadPool::get_next_loop(){
    assert(m_started);
    m_baseLoop->assert_in_loop_thread();

    EventLoop* loop = m_baseLoop;
    if(!m_loops.empty()){
        loop = m_loops[m_next];
        ++ m_next;
        if(m_next > m_loops.size()){
            m_next = 0;
        }
    }

    return loop;
}

