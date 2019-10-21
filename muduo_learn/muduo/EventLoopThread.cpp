#include "muduo/EventLoopThread.h"
#include "muduo/EventLoop.h"

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb) : 
    m_loop(nullptr),
    m_exiting(false),
    m_thread(std::bind(&EventLoopThread::thread_func, this)),
    m_callback(cb)
{
    std::unique_lock<std::mutex> ul(m_mutex);
    while(m_loop == nullptr){
        m_cond.wait(ul);
    }
}

EventLoopThread::~EventLoopThread(){
    m_exiting = true;
    m_loop->quit();
    m_thread.join();
}

void EventLoopThread::thread_func(){
    EventLoop loop;

    if(m_callback){
        m_callback(&loop);
    }

    m_mutex.lock();
    m_loop = &loop;
    m_mutex.unlock();

    m_cond.notify_one();

    loop.loop();
}