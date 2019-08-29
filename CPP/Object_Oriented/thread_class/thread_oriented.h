/**
 * 线程类基类
 * 使用面向对象方式对POSIX线程pthread进行封装
 * 包含抽象方法，不能进行实例化
*/

#ifndef THREAD_ORIENTED_H_
#define THREAD_ORIENTED_H_

#include <pthread.h>

class Thread{
public:
    Thread();

    virtual ~Thread();

    void start();

    void join();

private:
    static void * thread_entry_func(void * arg);

    virtual void run() = 0;

private:
    pthread_t m_tid;
};

#endif // THREAD_ORIENTED_H_