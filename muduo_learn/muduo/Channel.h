#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <memory>
#include <functional>

#include <tools/base/noncopyable.h>
#include <tools/base/Timestamp.h>

class EventLoop;

class Channel : noncopyable {
public:
    typedef std::function<void()> EventCallback;                // 事件回调函数类型
    typedef std::function<void(Timestamp)> ReadEventCallback;   // 读事件回调函数类型，需要时间戳参数

    Channel(EventLoop * loop, int fd);
    ~Channel();

    // get_xx is_xx set_xx
    void set_read_callback(const ReadEventCallback& rcb){
        m_readCallback = rcb;
    }

    void set_write_callback(const EventCallback& wcb){
        m_writeCallback = wcb;
    }

    void set_close_callback(const EventCallback& ccb){
        m_closeCallback = ccb;
    }

    void set_error_callback(const EventCallback& ecb){
        m_errorCallback = ecb;
    }

    int get_fd() const {
        return m_fd;
    }

    int get_events() const {
        return m_events;
    }

    bool is_none_events() const {
        return m_events == k_noneEvent;
    }

    bool is_write_events() const {
        return m_events& k_writeEvent;
    }

    int get_revents() const {
        return m_revents;
    }

    void set_revents(int revents){
        m_revents = revents;
    }

    // for Poller
    int get_index() const {
        return m_index;
    }

    void set_index(int index){
        m_index = index;
    }

    EventLoop* get_owner_loop() const {
        return m_loop;
    }

    void handle_event(Timestamp receiveTime);

    // 将被shared_ptr管理的channel持有者与该channel绑定
    // 防止channel持有者在处理事件中被销毁
    void tie(const std::shared_ptr<void>&);

    void enable_reading(){
        m_events |= k_readEvent;
        update();
    }

    void disable_reading(){
        m_events &= ~k_readEvent;
        update();
    }

    void enable_writing(){
        m_events |= k_writeEvent;
        update();
    }

    void disable_writing(){
        m_events &= ~k_writeEvent;
        update();
    }

    void disable_all(){
        m_events = k_noneEvent;
        update();
    }

    void remove();

private:
    void handle_event_with_guard(Timestamp receiveTime);

    void update();

private:
    static const int k_noneEvent;   // 空事件常量
    static const int k_readEvent;   // 读事件常量
    static const int k_writeEvent;  // 写事件常量

    EventLoop* m_loop; // 记录所属的EventLoop对象
    const int m_fd;     // 负责的文件描述符，不负责关闭该fd
    int m_events;       // 关注的事件
    int m_revents;      // poll/epoll返回的事件
    int m_index;        // used by Poller，既用来表征在m_pollfdList中的位置又用于区分add/update操作
    bool m_logHup;      // ??

    std::weak_ptr<void> m_tie;  // 指向绑定的对象
    bool m_tied;                // 记录该Channel对象是否已经绑定
    bool m_eventHandling;       // 是否正在处理事件

    ReadEventCallback m_readCallback;   // 读事件回调函数
    EventCallback m_writeCallback;      // 写事件回调函数
    EventCallback m_closeCallback;      // 连接关闭事件回调函数
    EventCallback m_errorCallback;      // 错误事件回调函数
};

#endif // CHANNEL_H_