#include <pthread.h> // pthread_create

#include "ngx_log.h"
#include "ngx_macro.h"
#include "ngx_global.h"
#include "ngx_c_conf.h"
#include "ngx_c_threadPool.h"
#include "ngx_c_memoryPool.h"

ThreadPool * ThreadPool::instance = nullptr;
pthread_mutex_t ThreadPool::m_msgQueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ThreadPool::m_cond = PTHREAD_COND_INITIALIZER;
std::atomic<bool> ThreadPool::m_stop = false;
std::atomic<int> ThreadPool::m_runningNum = 0;

ThreadPool::ThreadPool(){}

ThreadPool::~ThreadPool(){
    // 线程池停止工作
    ngx_threadPool_stop();

    // 清空消息队列
    ngx_msgQue_clear();
}

int ThreadPool::ngx_threadPool_create(){
    ConfFileProcessor * cfp = ConfFileProcessor::getInstance();
    int threadNum = cfp->ngx_conf_getContent_int("ThreadNum", NGX_THREAD_NUM);

    ThreadItem * newItem = nullptr;
    int err = -1;
    for(int i = 0; i < threadNum; ++ i){
        newItem = new ThreadItem;
        m_threadVec.push_back(newItem);

        err = pthread_create(&(newItem->tid), NULL, ngx_thread_entryFunc, newItem);
        if(err != 0){ // 创建线程出错
            ngx_log(NGX_LOG_ERR, err, "ngx_create_threadPool()函数中创建线程失败，i = %d", i);
            return -1;
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

        while(m_stop == false && m_msgQueue.size() == 0){
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
}

void ThreadPool::ngx_msgQue_push(uint8_t * msg){
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

/**
 * @brief 清空消息队列
 * 调用时线程池已经停止工作，因此不需要考虑互斥
 */
void ThreadPool::ngx_msgQue_clear(){
    MemoryPool * mp = MemoryPool::getInstance();
    uint8_t * msg = nullptr;

    while(!m_msgQueue.empty()){
        msg = m_msgQueue.front();
        m_msgQueue.pop_front(); // 移除队首的消息
        mp->ngx_free_memory(msg);
    }
}