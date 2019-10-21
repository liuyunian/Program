#ifndef EVENTLOOPTHREAD_H_
#define EVENTLOOPTHREAD_H_

#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>

#include <tools/base/noncopyable.h>

class EventLoop;

class EventLoopThread : noncopyable {
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallback;

    EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback());       // 这里是什么操作？
    ~EventLoopThread();

    EventLoop* get_loop() const {
        return m_loop;
    }

private:
    void thread_func();

private:
    EventLoop* m_loop;
    bool m_exiting;

    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_cond;

    ThreadInitCallback m_callback;
};

#endif // EVENTLOOPTHREAD_H_