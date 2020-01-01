#include <sstream>

#include <poll.h>   // poll POLLIN ...
#include <assert.h> // assert

#include <tools/log/log.h>

#include "muduo/Channel.h"
#include "muduo/EventLoop.h"

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop *loop, int fd) : 
  m_loop(loop),
  m_fd(fd),
  m_addedToLoop(false),
  m_tied(false),
  m_eventHandling(false),
  m_events(0),
  m_revents(0),
  m_index(-1)
{}

Channel::~Channel(){
  assert(!m_eventHandling);
  assert(!m_addedToLoop);

  if(m_loop->is_in_loop_thread()){
    assert(!m_loop->has_channel(this));
  }
}

void Channel::tie(const std::shared_ptr<void>& obj){
  m_tie = obj;
  m_tied = true;
}

void Channel::handle_event(Timestamp receiveTime){
  std::shared_ptr<void> guard;
  if(m_tied){                                     // 该Channel对象已经绑定
    guard = m_tie.lock();                       // 获取std::weak_ptr监视的shared_ptr
    
    if(guard){
        handle_event_with_guard(receiveTime);
    }
  }
  else{                                           // 该Channel对象没有绑定owner情况下，也执行事件分发
    handle_event_with_guard(receiveTime);
  }
}

void Channel::handle_event_with_guard(Timestamp receiveTime){
  m_eventHandling = true;

  if((m_revents & POLLHUP) && !(m_revents & POLLIN)){                 // POLLHUP事件并且不是POLLIN事件
    LOG_WARN("fd = %d, Channel::handle_event() POLLHUP", m_fd);

    if(m_closeCallback){
      m_closeCallback();
    }
  }

  if(m_revents & POLLNVAL){                                           // POLLNVAL事件：文件描述符不是一个打开的文件
    LOG_WARN("fd = %d, Channel::handle_event() POLLNVAL", m_fd);
    
    if(m_errorCallback){
      m_errorCallback();
    }
  }

  if(m_revents & POLLERR){                                            // POLLERR事件
    if(m_errorCallback){
      m_errorCallback();
    }
  }

  if(m_revents & (POLLIN | POLLPRI | POLLRDHUP)){                     // POLLIN可读事件 | POLLPRI高优先级可读事件 | POLLRDHUP连接对方关闭连接
    if(m_readCallback){
      m_readCallback(receiveTime);
    }
  }

  if(m_revents & POLLOUT){                                            // POLLOUT可写事件
    if(m_writeCallback){
      m_writeCallback();
    }
  }

  m_eventHandling = false;
}

void Channel::update(){
  m_addedToLoop = true;
  m_loop->update_channel(this);
}

void Channel::remove(){
  assert(is_none_events());   // 所以一般在调用remove之前都要先调用disable_all()
  m_addedToLoop = false;
  m_loop->remove_channel(this);
}

std::string Channel::revents_to_string() const {
  return events_to_string(m_revents);
}

std::string Channel::events_to_string() const {
  return events_to_string(m_events);
}

std::string Channel::events_to_string(int events) const {
  std::ostringstream oss;
  oss << m_fd << ": ";
  if (events & POLLIN)
  oss << "IN ";
  if (events & POLLPRI)
  oss << "PRI ";
  if (events & POLLOUT)
  oss << "OUT ";
  if (events & POLLHUP)
  oss << "HUP ";
  if (events & POLLRDHUP)
  oss << "RDHUP ";
  if (events & POLLERR)
  oss << "ERR ";
  if (events & POLLNVAL)
  oss << "NVAL ";

  return oss.str();
}

