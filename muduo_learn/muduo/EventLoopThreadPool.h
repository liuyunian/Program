#ifndef EVENTLOOPTHREADPOOL_H_
#define EVENTLOOPTHREADPOOL_H_

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include <tools/base/noncopyable.h>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : noncopyable {
public:
    EventLoopThreadPool(EventLoop* baseLoop);
    ~EventLoopThreadPool();

    bool is_started() const {
        return m_started;
    }

    void set_number_of_threads(int numThreads){
        m_numThreads = numThreads;
    }

    void start();

    /**
     * 轮叫的方式调度事件循环
     * 在start()执行之后调用时有效的
    */
    EventLoop* get_next_loop();

private:
    bool m_started;
    int m_numThreads;
    int m_next;

    EventLoop* m_baseLoop;                                      // MainLoop
    std::vector<std::unique_ptr<EventLoopThread>> m_threads;    // EventLoop线程集合 
    std::vector<EventLoop*> m_loops;                            // 记录每个EventLoop线程中EventLoop对象指针
};

#endif // EVENTLOOPTHREADPOOL_H_