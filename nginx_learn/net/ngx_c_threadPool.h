/**
 * 线程池
 * 采用POSIX thread标准实现
 * 单例模式
 */

#ifndef NGX_C_THREADPOOL_H_
#define NGX_C_THREADPOOL_H_

#include <vector>
#include <queue>
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
    void ngx_recvMsgQue_push(uint8_t * msg);

private:
    static void * ngx_thread_entryFunc(void * arg);

private:
    static pthread_mutex_t m_recvMsgQueMutex; // 消息队列互斥量
    static pthread_cond_t m_cond; // 条件变量
    static std::atomic<bool> m_stop; // 线程池是否已经停止工作
    static std::atomic<int> m_runningNum; // 记录正在运行的线程数

    /**
     * 接收消息队列
     * 这里选用queue容器，老师的课中选用的list容器
     * queue容器的效率要比list容器高
     * 目前代码中对接收消息的处理符合“先入先出”的原则，如果后续要对接收消息具有优先级的划分的话就需要更换容器了
    */
    static std::queue<uint8_t *> m_recvMsgQue;

    int m_threadNum; // 线程池中的线程数目
    std::vector<ThreadItem *> m_threadVec;
};

#endif // NGX_C_THREADPOOL_H_