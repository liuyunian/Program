#include <iostream>

#include <unistd.h>

#include <muduo/net/EventLoop.h>
#include <muduo/net/EventLoopThread.h>
#include <tools_cxx/CurrentThread.h>

void print(){
    std::cout << "print(): pid = " << getpid() << ", tid = " << CurrentThread::get_tid() << std::endl;
}

int main(){
    std::cout << "main(): pid = " << getpid() << ", tid = " << CurrentThread::get_tid() << std::endl;

    EventLoopThread loopThread;
    EventLoop * loop = loopThread.get_eventLoop();

    loop->run_in_loop(print);
    sleep(1);

    loop->run_after(2, print);
    sleep(3);

    return 0;
}