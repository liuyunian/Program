/**
 * 线程池
 * 采用POSIX thread标准实现
 * 单例模式
 */

#ifndef NGX_C_THREADPOOL_H_
#define NGX_C_THREADPOOL_H_

#include <vector>
#include <list>
#include <atomic>

#include <stddef.h> // size_t
#include <pthread.h> // pthread_t

class ThreadPool{
private: 
    ThreadPool();

    ~ThreadPool();

    static ThreadPool * instance;

    class GCInstance{
    public:
        ~GCInstance(){
            if(ThreadPool::instance != nullptr){
                delete ThreadPool::instance;
                ThreadPool::instance = nullptr;
            }
        }
    };

    /**
     * @brief 代表一个线程
     */
    struct ThreadItem{
        pthread_t tid;
        std::atomic<bool> isRunning;

        ThreadItem() : isRunning(false){}
    };

public:
    static ThreadPool * getInstance(){
        if(instance == nullptr){
            instance = new ThreadPool();
            static GCInstance gc; 
        }

        return instance;
    }

    int ngx_threadPool_create();

    void ngx_threadPool_stop();

    // 消息队列
    void ngx_msgQue_push(uint8_t * msg);

private:
    static void * ngx_thread_entryFunc(void * arg);

private:
    static pthread_mutex_t m_msgQueMutex; // 消息队列互斥量
    static pthread_cond_t m_cond; // 条件变量
    static std::atomic<bool> m_stop; // 线程池是否已经停止工作
    static std::atomic<int> m_runningNum; // 记录正在运行的线程数
    static std::list<uint8_t *> m_msgQueue; // 消息队列

    int m_threadNum; // 线程池中的线程数目
    std::vector<ThreadItem *> m_threadVec;
};

#endif // NGX_C_THREADPOOL_H_