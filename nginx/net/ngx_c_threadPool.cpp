#include <pthread.h> // pthread_create
#include <unistd.h> // usleep

#include "ngx_c_threadPool.h"

#include "app/ngx_log.h"
#include "app/ngx_c_conf.h"
#include "_include/ngx_macro.h"
#include "_include/ngx_global.h"
#include "misc/ngx_c_memoryPool.h"

ThreadPool * ThreadPool::instance = nullptr;
pthread_mutex_t ThreadPool::m_msgQueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ThreadPool::m_cond = PTHREAD_COND_INITIALIZER;
std::atomic<bool> ThreadPool::m_stop (false);
std::atomic<int> ThreadPool::m_runningNum(0);
std::list<uint8_t *> ThreadPool::m_msgQueue;

ThreadPool::ThreadPool() : m_threadNum(0){}

ThreadPool::~ThreadPool(){
    // 线程池停止工作
    ngx_threadPool_stop();
}

int ThreadPool::ngx_threadPool_create(){
    ConfFileProcessor * cfp = ConfFileProcessor::getInstance();
    m_threadNum = cfp->ngx_conf_getContent_int("ThreadNum", NGX_THREAD_NUM);

    ThreadItem * newItem = nullptr;
    int err = -1;
    for(int i = 0; i < m_threadNum; ++ i){
        newItem = new ThreadItem;
        m_threadVec.push_back(newItem);

        err = pthread_create(&(newItem->tid), NULL, ngx_thread_entryFunc, newItem);
        if(err != 0){ // 创建线程出错
            ngx_log(NGX_LOG_ERR, err, "ngx_create_threadPool()函数中创建线程失败，i = %d", i);
            return -1;
        }
    }

    // 必须保证创建的线程都运行到pthread_cond_wait()休眠之后，本函数才可以返回
for_label:
    for(auto & thread : m_threadVec){
        if(thread->isRunning == false){
            usleep(10 * 1000); // 休眠10ms
            goto for_label;
        }
    }
 
    return 0;
}

void * ThreadPool::ngx_thread_entryFunc(void * arg){
    ThreadItem * thread = static_cast<ThreadItem *>(arg);

    int err = -1; // 错误码
    uint8_t * msg = nullptr;
    MemoryPool * mp = MemoryPool::getInstance();
    
    for(;;){
        err = pthread_mutex_lock(&m_msgQueMutex);
        if(err != 0){
            ngx_log(NGX_LOG_ERR, err, "ngx_thread_entryFunc()函数中互斥量m_msgQueMutex加锁失败");
        }

        while(m_stop == false && m_msgQueue.empty()){
            if(thread->isRunning == false){
                thread->isRunning = true;
            }

            pthread_cond_wait(&m_cond, &m_msgQueMutex);
        }

        if(m_stop){ // 用于stop
            err = pthread_mutex_unlock(&m_msgQueMutex);
            if(err != 0){
                ngx_log(NGX_LOG_ERR, err, "ngx_thread_entryFunc()函数中互斥量m_msgQueMutex解锁失败");
            }

            break;
        }

        msg = m_msgQueue.front(); // 从消息队列中拿消息
        m_msgQueue.pop_front(); 

        err = pthread_mutex_unlock(&m_msgQueMutex);
        if(err != 0){
            ngx_log(NGX_LOG_ERR, err, "ngx_thread_entryFunc()函数中互斥量m_msgQueMutex解锁失败");
        }

        ++ m_runningNum;

        // ... 增加处理从消息队列中取出的消息的函数
        g_sock.ngx_msg_handle(msg);

        mp->ngx_free_memory(msg);
        -- m_runningNum;
    }

    return (void *)0;
}

void ThreadPool::ngx_threadPool_stop(){
    if(m_stop){ // 确保不被重复调用
        return;
    }

    // [1] 修改m_stop
    m_stop = true;

    // [2] 唤醒所有线程
    int err = pthread_cond_broadcast(&m_cond);
    if(err != 0){
        ngx_log(NGX_LOG_ERR, err, "ngx_threadPool_stop()函数中调用pthread_cond_broadcast()失败");
        return;
    }

    // [3] 获取线程的终止状态，并销毁ThreadItem对象
    for(auto & thread : m_threadVec){
        pthread_join(thread->tid, NULL);

        delete thread;
    }
    m_threadVec.clear();

    // [4] 清空消息队列
    MemoryPool * mp = MemoryPool::getInstance();
    uint8_t * msg = nullptr;

    while(!m_msgQueue.empty()){
        msg = m_msgQueue.front();
        m_msgQueue.pop_front(); // 移除队首的消息
        mp->ngx_free_memory(msg);
    }
}

void ThreadPool::ngx_msgQue_push(uint8_t * msg){
    ngx_log(NGX_LOG_DEBUG, 0, "执行了ngx_msgQue_push()函数");

    int err = -1; // 错误码
    err = pthread_mutex_lock(&m_msgQueMutex);
    if(err != 0){
        ngx_log(NGX_LOG_ERR, err, "ngx_msgQue_push()函数中互斥量m_msgQueMutex加锁失败");
    }

    m_msgQueue.push_back(msg);

    err = pthread_mutex_unlock(&m_msgQueMutex);
    if(err != 0){
        ngx_log(NGX_LOG_ERR, err, "ngx_msgQue_push()函数中互斥量m_msgQueMutex解锁失败");
    }

    err = pthread_cond_signal(&m_cond);
    if(err != 0){
        ngx_log(NGX_LOG_ERR, err, "ngx_threadPool_call()函数中调用pthread_cond_signal()失败");
        return;
    }

    if(m_threadNum == m_runningNum){ // 表示线程池中的线程都在运行，表示线程不够用了
        ngx_log(NGX_LOG_INFO, 0, "当前线程池中的线程都处于工作状态，需要考虑对线程池进行扩容");
    }
}