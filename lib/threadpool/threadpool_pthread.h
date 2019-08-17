/**
 * 线程池
 * 采用POSIX线程标准，不涉及模板
 * 一个进程中只需要一个线程池即可，所以这里采用单例类方式实现
*/
#ifndef THREADPOOL_PTHREAD_H_
#define THREADPOOL_PTHREAD_H_

#include <vector>
#include <queue> 

#include <pthread.h>

#define THREADS 10

class ThreadPool{
private:
    ThreadPool();

    ~ThreadPool();

    static ThreadPool * m_instance;

    struct GCInstance{
        ~GCInstance(){
            if(m_instance != nullptr){
                delete m_instance;
                m_instance = nullptr;
            }
        }
    };

    struct ThreadItem{
        pthread_t tid;
        std::atomic_bool isRunning;

    public:
        ThreadItem() : isRunning(false){}
    };

    struct Task{
        void * (*func) (void *); // 函数指针
        void * arg; // 参数
    };

public:
    static ThreadPool * getInstance();

    /**
     * @brief 创建线程池
    */
    void threadpool_create();

    void threadpool_call(void *(* func)(void *), void * arg);

    void threadpool_stop();

private:
    static void * thread_entryFunc(void * arg);

private:
    static pthread_mutex_t m_mutex;
    static pthread_cond_t m_cond;
    static std::atomic<bool> m_stop; // 线程池是否停止工作
    static std::atomic<int> m_runningNum; // 记录正在运行的线程数，用于调整线程池大小
    static std::queue<Task *> m_taskQue; // 任务队列

    int m_threadNum;
    std::vector<ThreadItem *> m_threadVec;
};

#endif // THREADPOOL_PTHREAD_H_