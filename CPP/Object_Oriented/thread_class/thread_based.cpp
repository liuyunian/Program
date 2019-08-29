/**
 * 线程类类
 * 使用基于对象编程方式对POSIX线程pthread进行封装
*/

#include <iostream>

#include "thread_based.h"

Thread::Thread(ThreadFunc callback) : 
    m_callback(callback){}

Thread::~Thread(){}

void Thread::start(){
    pthread_create(&m_tid, NULL, thread_entry_func, this);
}

void Thread::join(){
    pthread_join(m_tid, NULL);
}

void * Thread::thread_entry_func(void * arg){
    Thread * tThis = static_cast<Thread *>(arg);

    tThis->m_callback();

    return (void *)0;
}