#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_

#include <memory>
#include <atomic>
#include <mutex>
#include <vector>
#include <functional>

#include <stdlib.h>

#include <tools/base/Timestamp.h>
#include <tools/base/noncopyable.h>
#include <tools/base/CurrentThread.h>

#include "muduo/TimerId.h"
#include "muduo/Callbacks.h"

class Channel;
class Poller;
class TimerQueue;

class EventLoop : noncopyable{
public:
    typedef std::function<void()> Functor;

    EventLoop();
    ~EventLoop();

    /**
     * muduo库中有些成员函数是线程安全的，可以跨线程调用；有些成员函数只能在特定的IO线程中执行
     * is_in_loop_thread()和assert_in_loop_thread()用于在运行时检查某些函数是否在特定的IO线程中执行
    */
    bool is_in_loop_thread(){
        return m_threadId == CurrentThread::get_tid();
    }

    void assert_in_loop_thread();

    /**
     * 运行事件循环
     * loop()函数只能在EventLoop对象所在的IO线程中执行
    */
    void loop();

    /**
     * 停止运行事件循环
     * quit()函数可以跨线程调用
    */
    void quit();

    // call in other thread
    void run_in_loop(const Functor& cb);
    void queue_in_loop(const Functor& cb);

    // timer
    TimerId run_at(const Timestamp time, const TimerCallback& cb);
    TimerId run_after(double delay, const TimerCallback& cb);
    TimerId run_every(double interval, const TimerCallback& cb);
    void cancel(TimerId timerId);

    // internal usage
    void update_channel(Channel* channel);  // 在Poller中添加或者更新该Channel对象
    void remove_channel(Channel* channel);  // 在Poller中移除该Channel对象
    bool has_channel(Channel* channel);     // 判断Poller中是否有该Channel对象

    void wakeup();

private:
    void handle_read();

    void handle_pending_functors();

private:    
    bool m_looping;                             // 是否处于事件循环中，仅在特定的IO线程中使用，所以不必是atomic
    std::atomic<bool> m_quit;                   // 事件循环是否停止，可能会跨线程使用，所以必须是atomic
    bool m_eventHandling;                       // 是否正在处理事件，
    bool m_callingPendingFunctors;              // 是否正在处理Pending Functors
    const int m_threadId;                       // 所处的线程tid

    Timestamp m_pollReturnTime;                 // 记录m_poller->poll()函数返回的时间戳
    std::unique_ptr<Poller> m_poller;           // IO复用对象
    std::vector<Channel*> m_activeChannels;     // Poller返回的活动通道
    Channel* m_currentActiveChannel;            // 当前正在处理的活动通道

    int m_eventfd;                              // 用于事件唤醒
    std::unique_ptr<Channel> m_wakeupChannel;   // eventfd所属的Channel
    std::mutex m_mutex;                         
    std::vector<Functor> m_pendingFunctors;     // 用于IO线程执行其他非IO型任务

    std::unique_ptr<TimerQueue> m_timerQueue;   // 定时器队列
};

#endif // EVENTLOOP_H_