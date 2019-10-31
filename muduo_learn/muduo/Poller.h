#ifndef POLLER_H_
#define POLLER_H_

#include <vector>
#include <map>

#include <tools/base/noncopyable.h>
#include <tools/base/Timestamp.h>

#include "muduo/EventLoop.h"

class Channel;

/**
 * IO复用基类
*/
class Poller : noncopyable{ // 该类不拥有Channel对象
public:
    typedef std::vector<Channel*> ChannelList;

    Poller(EventLoop* loop);
    virtual ~Poller() = default;                                                // 虚析构

    /**
     * @brief 阻塞等待IO事件发生，必须在IO线程中调用
     * @param timeoutMs 指定阻塞时间
     * @param activeChannels 记录当前发生事件的Channel
    */
    virtual Timestamp poll(int timeoutMs,  ChannelList* activeChannels) = 0;    // 纯虚函数

    /**
     * @brief 改变感兴趣的IO事件，必须在IO线程中调用
    */
    virtual void update_channel(Channel* channel) = 0;                          // 纯虚函数

    /**
     * @brief 移除Channel，必须在IO线程中调用
    */
    virtual void remove_channel(Channel* channel) = 0;                          // 纯虚函数

    virtual bool has_channel(Channel* channel) const;                           // 虚函数

    /**
     * @brief 创建默认的Poller实例
    */
    static Poller* new_default_Poller(EventLoop * loop);

protected:
    EventLoop* m_ownerLoop;
    std::map<int, Channel*> m_channelStore;
};

#endif // POLLER_H_