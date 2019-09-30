#include <algorithm>
#include <iostream>

#include <poll.h>           // poll
#include <assert.h>         // assert
#include <sys/eventfd.h>    // eventfd
#include <unistd.h>         // read write

#include <tools_cxx/log.h>

#include "EventLoop.h"
#include "Channel.h"
#include "Poller.h"
#include "TimerQueue.h"

__thread EventLoop * t_loopInThisThread = nullptr;

const int k_pollTimeoutMs = 10000; // 10秒

static int create_eventfd(){
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0){
        LOG_SYSFATAL("Failed to create eventfd");
    }

    return evtfd;
}

EventLoop::EventLoop() : 
    m_looping(false),
    m_quit(false),
    m_eventHandling(false),
    m_callingPendingFunctors(false),
    m_threadID(CurrentThread::get_tid()),
    m_poller(Poller::new_default_Poller(this)),
    m_timerQueue(new TimerQueue(this)),
    m_wakeupFd(create_eventfd()),
    m_wakeupChannel(new Channel(this, m_wakeupFd)),
    m_currentActiveChannel(nullptr)
{
    LOG_INFO("EventLoop %p created in thread %d", this, m_threadID);
 
    if(t_loopInThisThread != nullptr){
        LOG_FATAL("Another EventLoop exists in this thread %d", m_threadID);
    }
    else{
        t_loopInThisThread = this;
    }

    m_wakeupChannel->set_readCallback(std::bind(&EventLoop::handle_wakeupFd_readEvent, this));
    m_wakeupChannel->enable_reading();
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

        do_pendingFunctors();
    }

    LOG_INFO("EventLoop %p stop looping", this);
    m_looping = false;
}

void EventLoop::quit(){ // 可以跨线程调用
    m_quit = true;
    if(!is_in_loopThread()){
        wakeup();
    }
}

void EventLoop::run_in_loop(const Functor& cb){
    if(is_in_loopThread()){ // 如果是当前IO线程调用run_in_loop()，则同步执行回调函数cb
        cb();
    }
    else{                   // 如果是其他线程调用run_in_loop()，则异步的将回调函数添加到m_pendingFunctors队列中
        queue_in_loop(cb);
    }
}

void EventLoop::queue_in_loop(const Functor& cb){
    m_mutex.lock();
    m_pendingFunctors.push_back(cb);
    m_mutex.unlock();

    // 调用queue_in_loop()的线程不是IO线程，需要wakeup
    // 调用queue_in_loop()的线程是IO线程，但是此时正在处理pending functor，需要wakeup
    // 只有IO线程的事件回调中调用queue_in_loop才不需要唤醒
    if(!is_in_loopThread() || m_callingPendingFunctors){
        wakeup();
    }
}

TimerId EventLoop::run_at(const Timestamp time, const TimerCallback & cb){
    return m_timerQueue->add_timer(cb, time, 0);
}

TimerId EventLoop::run_after(double delay, const TimerCallback & cb){
    Timestamp time(add_time(Timestamp::now(), delay));
    return run_at(time, cb);
}

TimerId EventLoop::run_every(double interval, const TimerCallback & cb){
    Timestamp time(add_time(Timestamp::now(), interval));
    return m_timerQueue->add_timer(cb, time, interval);
}

void EventLoop::cancel(TimerId timerId){
    m_timerQueue->cancel(timerId);
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

void EventLoop::wakeup(){
    uint64_t one = 1;
    ssize_t n = ::write(m_wakeupFd, &one, sizeof one);
    if (n != sizeof one){
        LOG_SYSERR("EventLoop::wakeup() writes %d bytes instead of 8", n);
    }
}

void EventLoop::handle_wakeupFd_readEvent(){
    uint64_t one;
    ssize_t n = ::read(m_wakeupFd, &one, sizeof one);
    if (n != sizeof one){
        LOG_SYSERR("EventLoop::handle_wakeupFd_readEvent() reads %d bytes instead of 8", n);
    }
}

void EventLoop::do_pendingFunctors(){
    std::vector<Functor> functors;

    m_callingPendingFunctors = true;

    m_mutex.lock();
    functors.swap(m_pendingFunctors);
    m_mutex.unlock();

    for(auto & pendingFunctor : functors){
        pendingFunctor();
    }

    m_callingPendingFunctors = false;
}