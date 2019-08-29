/**
 * 线程类类
 * 使用基于对象编程方式对POSIX线程pthread进行封装
*/

#ifndef THREAD_BASED_H_
#define THREAD_BASED_H_

#include <functional>

#include <pthread.h>

class Thread{
public:
    typedef std::function<void()> ThreadFunc;

    Thread(ThreadFunc callback);

    ~Thread();

    void start();

    void join();

private:
    static void * thread_entry_func(void * arg);

private:
    ThreadFunc m_callback;
    pthread_t m_tid;
};

#endif // THREAD_BASED_H_