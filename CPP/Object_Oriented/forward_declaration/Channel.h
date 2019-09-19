#ifndef CHANNEL_H_
#define CHANNEL_H_

#include "EventLoop.h"

class Channel{
public:
    Channel(EventLoop * loop) : m_loop(loop) {
        std::cout << "执行了Channel构造函数" << std::endl;
    }

    ~Channel(){
        std::cout << "执行了Channel析构函数" << std::endl;
    }

private:
    EventLoop * m_loop;
};

#endif // CHANNEL_H_