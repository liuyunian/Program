#include "muduo/EventLoopThread.h"
#include "muduo/EventLoop.h"

EventLoopThread::EventLoopThread() : 
    m_loop(nullptr),
    m_exiting(false),
    m_thread(nullptr){}

EventLoopThread::~EventLoopThread(){
    m_exiting = true;
    if(m_loop != nullptr){
        m_loop->quit();
        m_thread->join();
    }
}

EventLoop* EventLoopThread::start_loop(){
    m_thread.reset(new std::thread(std::bind(&EventLoopThread::thread_func, this)));

    EventLoop* loop = nullptr;
    {
        std::unique_lock<std::mutex> ul(m_mutex);
        while(m_loop == nullptr){
            m_cond.wait(ul);
        }

        loop = m_loop;
    }

    return loop;
}

void EventLoopThread::thread_func(){
    EventLoop loop;
    {
        std::unique_lock<std::mutex> ul(m_mutex);
        m_loop = &loop;
    }
    m_cond.notify_one();

    loop.loop();

    std::unique_lock<std::mutex> ul(m_mutex);
    m_loop = nullptr;
}