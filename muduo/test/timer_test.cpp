#include <iostream>

#include <stdio.h>
#include <unistd.h> // sleep

#include <tools/base/Timestamp.h>
#include <tools/base/CurrentThread.h>

#include <muduo/TimerId.h>
#include <muduo/EventLoop.h>

int cnt = 0;
EventLoop * g_loop;

void printTid(){
    printf("pid = %d, tid = %d\n", getpid(), CurrentThread::get_tid());
    printf("now %s\n", Timestamp::now().to_string().c_str());
}

void print(const char* msg){
    printf("msg %s %s\n", Timestamp::now().to_string().c_str(), msg);
    if (++ cnt == 20){
        g_loop->quit();
    }
}

void cancel(TimerId timer){
  g_loop->cancel(timer);
  printf("cancelled at %s\n", Timestamp::now().to_string().c_str());
}

int main(){
    printTid();
    sleep(1);

    EventLoop loop;
    g_loop = &loop;

    print("main");
    loop.run_after(1, std::bind(print, "once1"));
    loop.run_after(1.5, std::bind(print, "once1.5"));
    loop.run_after(2.5, std::bind(print, "once2.5"));
    loop.run_after(3.5, std::bind(print, "once3.5"));
    TimerId t45 = loop.run_after(4.5, std::bind(print, "once4.5"));
    loop.run_after(4.2, std::bind(cancel, t45));
    loop.run_after(4.8, std::bind(cancel, t45));
    loop.run_every(2, std::bind(print, "every2"));
    TimerId t3 = loop.run_every(3, std::bind(print, "every3"));
    loop.run_after(9.001, std::bind(cancel, t3));

    loop.loop();
    print("main loop exits");
}