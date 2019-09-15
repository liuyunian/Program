#include <iostream>
#include <thread>

#include <unistd.h> // getpid
#include <tools_cxx/CurrentThread.h>

#include "EventLoop.h"

EventLoop * g_loop;

void threadFunc(){
    std::cout << "child thread, pid = " << getpid() << " tid = " << CurrentThread::get_tid() << std::endl;
    g_loop->loop();
}

int main(){
    std::cout << "main thread, pid = " << getpid() << " tid = " << CurrentThread::get_tid() << std::endl;
    g_loop = new EventLoop();

    std::thread t(threadFunc);
    
    t.join();
    return 0;
}