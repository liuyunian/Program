#include <iostream>

#include <unistd.h>

#include <muduo/EventLoop.h>
#include <muduo/EventLoopThread.h>
#include <tools/base/CurrentThread.h>

void print(){
    std::cout << "print(): pid = " << getpid() << ", tid = " << CurrentThread::get_tid() << std::endl;
}

int main(){
    std::cout << "main(): pid = " << getpid() << ", tid = " << CurrentThread::get_tid() << std::endl;

    EventLoopThread loopThread;
    EventLoop * loop = loopThread.start_loop();

    loop->run_in_loop(print);
    sleep(1);

    loop->run_after(2, print);
    sleep(3);

    return 0;
}