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

    EventLoop* get_owner_loop() const {
        return m_loop;
    }

    int get_fd() const {
        return m_fd;
    }

    int get_events() const {
        return m_events;
    }

    bool is_none_events() const {
        return m_events == kNoneEvent;
    }

    bool is_writing() const {
        return m_events & kWriteEvent;
    }

    bool is_reading() const {
        return m_events & kReadEvent;
    }

    int get_revents() const {
        return m_revents;
    }

    void set_revents(int revents){
        m_revents = revents;
    }
        
    int get_index() const { // for Poller
        return m_index;
    }

    void set_index(int index){
        m_index = index;
    }

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

    void enable_reading(){
        m_events |= kReadEvent;
        update();
    }

    void disable_reading(){
        m_events &= ~kReadEvent;
        update();
    }

    void enable_writing(){
      m_events |= kWriteEvent;
      update();
    }

    void disable_writing(){
      m_events &= ~kWriteEvent;
      update();
    }

    void disable_all(){
      m_events = kNoneEvent;
      update();
    }

    // 将被shared_ptr管理的channel持有者与该channel绑定
    // 防止channel持有者在处理事件中被销毁
    void tie(const std::shared_ptr<void>&);

    void handle_event(Timestamp receiveTime);

    /**
     * 从IO线程中移除该Channel，不再关注其负责的文件描述符事件
    */
    void remove();

    // for debug
    std::string revents_to_string() const;

    std::string events_to_string() const;

private:
    void handle_event_with_guard(Timestamp receiveTime);

    /**
     * 向IO线程中添加该Channel，开始关注文件描述符事件
    */
    void update();

    std::string events_to_string(int events) const;

private:
  static const int kNoneEvent;   // 空事件常量
  static const int kReadEvent;   // 读事件常量
  static const int kWriteEvent;  // 写事件常量

  EventLoop *m_loop;              // 记录所属的EventLoop对象
  const int m_fd;                 // 负责的文件描述符，不负责关闭该fd
  std::weak_ptr<void> m_tie;      // 指向绑定的对象

  bool m_addedToLoop;             // 记录该Channel对象是否已经添加到EventLoop中
  bool m_tied;                    // 记录该Channel对象是否已经绑定，绑定的对象真正持有该Channel对象
  bool m_eventHandling;           // 是否正在处理事件

  int m_events;                   // 所负责的文件描述符(fd)关注的事件
  int m_revents;                  // poll/epoll返回的发生事件
  int m_index;                    // used by Poller，既用来表征在m_pollfdList中的位置又用于区分add/update操作

  ReadEventCallback m_readCallback;   // 读事件回调函数
  EventCallback m_writeCallback;      // 写事件回调函数
  EventCallback m_closeCallback;      // 连接关闭事件回调函数
  EventCallback m_errorCallback;      // 错误事件回调函数
};

#endif // CHANNEL_H_