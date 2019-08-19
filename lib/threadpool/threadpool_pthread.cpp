/**
 * 线程池
 * 采用POSIX线程标准，不涉及模板
 * 一个进程中只需要一个线程池即可，所以这里采用单例类方式实现
 * 实现中没有对函数调用进行异常处理
*/

#include <iostream>

#include <unistd.h>
#include <pthread.h>

#include "threadpool_pthread.h"

ThreadPool * ThreadPool::m_instance;

pthread_mutex_t ThreadPool::m_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ThreadPool::m_cond = PTHREAD_COND_INITIALIZER;
std::atomic<bool> ThreadPool::m_stop(false); // 线程池是否停止工作
std::atomic<int> ThreadPool::m_runningNum(0); // 记录正在运行的线程数
std::queue<ThreadPool::Task *> ThreadPool::m_taskQue; // 任务队列

ThreadPool * ThreadPool::getInstance(){
    if(m_instance == nullptr){
        m_instance = new ThreadPool();
        static GCInstance gc;
    }

    return m_instance;
}

ThreadPool::ThreadPool() : m_threadNum(0){}

ThreadPool::~ThreadPool(){
    // 线程池停止工作
    threadpool_stop();
}

void ThreadPool::threadpool_create(){
    ThreadItem * newThread;

    m_threadNum = THREADS; // 这里可以由threadpool_create()函数参数传入或者最好的方式是通过配置文件指定
    for(int i = 0; i < m_threadNum; ++ i){
        newThread = new ThreadItem();
        m_threadVec.push_back(newThread);
        pthread_create(&newThread->tid, NULL, thread_entryFunc, newThread);
    }

for_label:
    for(auto & thread : m_threadVec){
        if(!thread->isRunning){
            usleep(10 * 1000); // 休眠10ms
            goto for_label;
        }
    }
}

void * ThreadPool::thread_entryFunc(void * arg){
    ThreadItem * thread = static_cast<ThreadItem *>(arg);

    for(;;){
        pthread_mutex_lock(&m_mutex);
        while(!m_stop && m_taskQue.empty()){
            if(!thread->isRunning){
                thread->isRunning = true;
            }

            pthread_cond_wait(&m_cond, &m_mutex);
        }

        if(m_stop){
            pthread_mutex_unlock(&m_mutex);
            break;
        }

        Task * t = m_taskQue.front();
        m_taskQue.pop();
        pthread_mutex_unlock(&m_mutex);

        ++ m_runningNum;

        t->func(t->arg); // 执行任务

        delete t;
        -- m_runningNum;
    }

    return (void *)0;
}

void ThreadPool::threadpool_call(void *(* func)(void *), void * arg){
    int err = -1; // 错误码

    Task * newTask = new Task;
    newTask->func = func;
    newTask->arg = arg;

    pthread_mutex_lock(&m_mutex);
    m_taskQue.push(newTask);
    pthread_mutex_unlock(&m_mutex);

    err = pthread_cond_signal(&m_cond);

    if(m_runningNum == m_threadNum){
        // 打印日志提醒线程池已满，需要扩充
    }
}

void ThreadPool::threadpool_stop(){
    if(m_stop){
        return;
    }

    m_stop = true;

    pthread_cond_broadcast(&m_cond);

    for(auto & thread : m_threadVec){
        pthread_join(thread->tid, NULL);

        delete thread;
    }
    m_threadVec.clear();

    Task * t;
    while(!m_taskQue.empty()){
        t = m_taskQue.front();
        m_taskQue.pop();
        delete t;
    }
}