#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_

#include <memory>
#include <atomic>
#include <vector>

#include <stdlib.h>
#include <tools_cxx/Timestamp.h>
#include <tools_cxx/noncopyable.h>
#include <tools_cxx/CurrentThread.h>

class Channel;
class Poller;

class EventLoop : noncopyable{
public:
    EventLoop();

    ~EventLoop();

    void loop();

    void quit();

    void update_channel(Channel * channel); // 在Poller中添加或者更新通道

    void remove_channel(Channel * channel); // 在Poller中移除通道

    void assert_in_loopThread(){
        if(!is_in_loopThread()){
            abort_not_in_loopThread();
        }
    }

    bool is_in_loopThread(){
        return m_threadID == CurrentThread::get_tid();
    }

private:
    void abort_not_in_loopThread();

private:    
    std::atomic<bool> m_looping;         // 是否处于事件循环中
    std::atomic<bool> m_quit;            // 事件循环是否停止
    std::atomic<bool> m_eventHandling;   // 是否正在处理事件

    const int m_threadID;

    Timestamp m_pollReturnTime;                 // 记录m_poller->poll()函数返回的时间戳
    std::unique_ptr<Poller> m_poller;           // IO复用对象
    std::vector<Channel *> m_activeChannels;    // Poller返回的活动通道
    Channel * m_currentActiveChannel;           // 当前正在处理的活动通道
};

#endif // EVENTLOOP_H_