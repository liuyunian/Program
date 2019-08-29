/**
 * 线程类基类
 * 使用面向对象方式对POSIX线程pthread进行封装
 * 包含抽象方法，不能进行实例化
*/

#include <iostream>

#include "thread_oriented.h"

Thread::Thread(){
    std::cout << "Thread() ..." << std::endl;
}

Thread::~Thread(){
    std::cout << "~Thread() ..." << std::endl;
}

void Thread::start(){
    pthread_create(&m_tid, NULL, thread_entry_func, this);
}

void Thread::join(){
    pthread_join(m_tid, NULL);
}

void * Thread::thread_entry_func(void * arg){
    Thread * tThis = static_cast<Thread *>(arg);

    tThis->run();

    return (void *)0;
}