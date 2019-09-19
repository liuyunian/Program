#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_

#include <iostream>
#include <vector>

// #include "Channel.h" // 编译失败
class Channel;          // 前向声明

class EventLoop{
public:
    EventLoop(){
        std::cout << "执行了EventLoop构造函数" << std::endl;
    }

    ~EventLoop(){
        std::cout << "执行了EventLoop析构函数" << std::endl;
    }

private:
    std::vector<Channel *> m_activeChannels;
};

#endif // EVENTLOOP_H_