#ifndef POLLER_H_
#define POLLER_H_

#include <vector>

#include <tools_cxx/noncopyable.h>
#include <tools_cxx/Timestamp.h>

#include "EventLoop.h"
#include "Channel.h"

/**
 * IO复用基类
*/
class Poller : noncopyable{ // 该类不拥有Channel对象
public:
    Poller(EventLoop * loop);

    virtual ~Poller() = default;

    /**
     * @brief 阻塞等待IO事件发生，必须在IO线程中调用
     * @param timeoutMs 指定阻塞时间
     * @param activeChannels ??
    */
    virtual Timestamp poll(int timeoutMs, std::vector<Channel *> * activeChannels) = 0; // 纯虚函数

    /**
     * @brief 改变感兴趣的IO事件，必须在IO线程中调用
     * @param channel 
    */
    virtual void update_channel(Channel * channel) = 0; // 纯虚函数

    /**
     * @brief 移除Channel，必须在IO线程中调用
     * @param channel
    */
    virtual void remove_channel(Channel * channel) = 0; // 纯虚函数

    void assert_in_loopThread(){
        m_ownerLoop->assert_in_loopThread();
    }

    /**
     * @brief 创建默认的Poller实例
    */
    static Poller * new_default_Poller(EventLoop * loop);

private:
    EventLoop * m_ownerLoop;
};

#endif // POLLER_H_