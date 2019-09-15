#include <iostream>
#include <thread>

#include <unistd.h> // getpid
#include <tools_cxx/CurrentThread.h>

#include "EventLoop.h"

void threadFunc(){
    std::cout << "child thread, pid = " << getpid() << " tid = " << CurrentThread::get_tid() << std::endl;
    EventLoop loop;
    loop.loop();
}

int main(){
    std::cout << "main thread, pid = " << getpid() << " tid = " << CurrentThread::get_tid() << std::endl;
    EventLoop loop;

    std::thread t(threadFunc);
    
    loop.loop();

    t.join();
    return 0;
}