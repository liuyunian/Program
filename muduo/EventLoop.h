#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_

#include <memory>
#include <atomic>
#include <mutex>
#include <vector>
#include <functional>

#include <stdlib.h>

#include <tools_cxx/log.h>
#include <tools_cxx/Timestamp.h>
#include <tools_cxx/noncopyable.h>
#include <tools_cxx/CurrentThread.h>

#include "TimerId.h"

class Channel;
class Poller;
class TimerQueue;

typedef std::function<void()> TimerCallback;

class EventLoop : noncopyable{
public:
    typedef std::function<void()> Functor;

    EventLoop();
    ~EventLoop();

    void loop();

    void quit();

    Timestamp get_pollReturnTime(){
        return m_pollReturnTime;
    }

    // call in other thread
    void run_in_loop(const Functor& cb);

    void queue_in_loop(const Functor& cb);

    // timer
    TimerId run_at(const Timestamp time, const TimerCallback & cb);

    TimerId run_after(double delay, const TimerCallback & cb);

    TimerId run_every(double interval, const TimerCallback & cb);

    void cancel(TimerId timerId);


    // internal usage
    void update_channel(Channel * channel); // 在Poller中添加或者更新通道

    void remove_channel(Channel * channel); // 在Poller中移除通道

    void assert_in_loopThread(){
        if(!is_in_loopThread()){
            LOG_FATAL("EventLoop %p was created in %d thread, but current thread is %d", this, m_threadID, CurrentThread::get_tid());
        }
    }

    bool is_in_loopThread(){
        return m_threadID == CurrentThread::get_tid();
    }

    void wakeup();

private:
    void handle_wakeupFd_readEvent();

    void do_pendingFunctors();

private:    
    std::atomic<bool> m_looping;                // 是否处于事件循环中
    std::atomic<bool> m_quit;                   // 事件循环是否停止
    std::atomic<bool> m_eventHandling;          // 是否正在处理事件
    std::atomic<bool> m_callingPendingFunctors; // 是否正在处理Pending Functors

    const int m_threadID;

    Timestamp m_pollReturnTime;                 // 记录m_poller->poll()函数返回的时间戳
    std::unique_ptr<Poller> m_poller;           // IO复用对象

    std::unique_ptr<TimerQueue> m_timerQueue;   // 定时器队列

    int m_wakeupFd;                             // 用于事件唤醒
    std::unique_ptr<Channel> m_wakeupChannel;

    std::vector<Channel *> m_activeChannels;    // Poller返回的活动通道
    Channel * m_currentActiveChannel;           // 当前正在处理的活动通道

    std::mutex m_mutex;                         // 用于IO线程执行其他非IO型任务
    std::vector<Functor> m_pendingFunctors;
};

#endif // EVENTLOOP_H_