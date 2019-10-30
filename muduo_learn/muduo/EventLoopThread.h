#ifndef EVENTLOOPTHREAD_H_
#define EVENTLOOPTHREAD_H_

#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>

#include <tools/base/noncopyable.h>

#include "muduo/Callbacks.h"

class EventLoop;

class EventLoopThread : noncopyable {
public:
    EventLoopThread();
    ~EventLoopThread();

    EventLoop* start_loop();

private:
    void thread_func();

private:
    EventLoop* m_loop;
    bool m_exiting;

    std::unique_ptr<std::thread> m_thread;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};

#endif // EVENTLOOPTHREAD_H_