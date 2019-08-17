/**
 * 该线程池实现源自github上开源的ThreadPool项目，将其修改为单例类方式
 * 使用C++11中提供的多线程语法和模板编程，具有更强的适用性并支持跨平台
*/
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool {
private:
    ThreadPool(size_t);

    ~ThreadPool();

    static ThreadPool * m_instance;

public:
    static ThreadPool * getInstance();

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>; // 后置返回类型

private:
    struct GCInstance{
        ~GCInstance(){
            if(ThreadPool::m_instance != nullptr){
                delete ThreadPool::m_instance;
                ThreadPool::m_instance = nullptr;
            }
        }
    };

private:
    // need to keep track of threads so we can join them
    std::vector< std::thread > workers;

    // the task queue
    std::queue< std::function<void()> > tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared< std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // don't allow enqueueing after stopping the pool
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task](){ (*task)(); });
    }
    condition.notify_one();
    return res;
}

#endif