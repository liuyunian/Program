#include <algorithm>
#include <iostream>

#include <poll.h>           // poll
#include <assert.h>         // assert
#include <sys/eventfd.h>    // eventfd
#include <unistd.h>         // read write close

#include <tools/log/log.h>

#include "muduo/EventLoop.h"
#include "muduo/Channel.h"
#include "muduo/Poller.h"
#include "muduo/TimerQueue.h"

__thread EventLoop* t_loopInThisThread = nullptr;

const int kPollTimeoutMs = 10000; // 10秒

static int create_eventfd(){
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if(evtfd < 0){
    LOG_SYSFATAL("Failed to create eventfd in create_eventfd");
  }

  return evtfd;
}

EventLoop::EventLoop() : 
  m_looping(false),
  m_quit(false),
  m_eventHandling(false),
  m_callingPendingFunctors(false),
  m_threadId(CurrentThread::get_tid()),
  m_poller(Poller::new_default_Poller(this)),
  m_timerQueue(new TimerQueue(this)),
  m_eventfd(create_eventfd()),
  m_wakeupChannel(new Channel(this, m_eventfd)),
  m_currentActiveChannel(nullptr)
{
  LOG_DEBUG("EventLoop %p created in thread %d", this, m_threadId);

  if(t_loopInThisThread != nullptr){
    LOG_FATAL("Another EventLoop exists in thread %d", m_threadId);
  }
  else{
    t_loopInThisThread = this;
  }

  /**
   * 不是说不要在构造函数中暴露this指针（注册回调）-- 其他线程可能会访问到一个不完整的对象
   * 注意这里事件循环还没有开始，所以eventfd的可读事件不会感知到，也就不会回调可读事件的处理函数
   * 存在多个IO线程的情况下，会不会eventfd的可读事件被其他IO线程感知到并回调其处理函数呢？
   * 不会的，m_wakeupChannel只属于当前EventLoop对象
  */
  m_wakeupChannel->set_read_callback(std::bind(&EventLoop::handle_read, this));
  m_wakeupChannel->enable_reading();
}

EventLoop::~EventLoop(){
    LOG_DEBUG("EventLoop %p of thread %d destructs in thread %d", this, m_threadId, CurrentThread::get_tid());
    m_wakeupChannel->disable_all();
    m_wakeupChannel->remove();
    ::close(m_eventfd);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop(){
    assert(!m_looping);
    assert_in_loop_thread();

    m_looping = true;
    m_quit = false;

    LOG_DEBUG("EventLoop %p start looping", this);

    while(!m_quit){
        m_activeChannels.clear();
        m_pollReturnTime = m_poller->poll(kPollTimeoutMs, &m_activeChannels);

        m_eventHandling = true;
        for(auto & ch : m_activeChannels){
            m_currentActiveChannel = ch;
            m_currentActiveChannel->handle_event(m_pollReturnTime);
        }
        m_currentActiveChannel = nullptr;
        m_eventHandling = false;

        handle_pending_functors();
    }

    LOG_DEBUG("EventLoop %p stop looping in thread %d", this, m_threadId);
    m_looping = false;
}

void EventLoop::quit(){
  m_quit = true;

  // 为什么只有跨线程调用时才调用wakeup()唤醒
  if(!is_in_loop_thread()){
      wakeup();
  }
}

void EventLoop::assert_in_loop_thread(){
  if(!is_in_loop_thread()){
    LOG_FATAL("EventLoop %p was created in %d thread, but current thread is %d", this, m_threadId, CurrentThread::get_tid());
  }
}

void EventLoop::run_in_loop(const Functor& cb){
  // 如果是当前IO线程调用run_in_loop()，则同步执行回调函数cb
  // 如果是其他线程调用run_in_loop()，则异步的将回调函数添加到m_pendingFunctors队列中
  if(is_in_loop_thread()){
    cb();
  }
  else{
    queue_in_loop(cb);
  }
}

void EventLoop::queue_in_loop(const Functor& cb){
  {
    std::unique_lock<std::mutex> ul(m_mutex);
    m_pendingFunctors.push_back(cb);
  }

  // 调用queue_in_loop()的线程不是IO线程，需要wakeup
  // 调用queue_in_loop()的线程是IO线程，但是此时正在处理pending functor，需要wakeup
  // 只有IO线程的事件回调中调用queue_in_loop才不需要唤醒
  if(!is_in_loop_thread() || m_callingPendingFunctors){
    wakeup();
  }
}

TimerId EventLoop::run_at(const Timestamp time, const TimerCallback& cb){
  return m_timerQueue->add_timer(cb, time, 0);
}

TimerId EventLoop::run_after(double delay, const TimerCallback& cb){
  return run_at(Timestamp::now().add_time(delay), cb);
}

TimerId EventLoop::run_every(double interval, const TimerCallback& cb){
    return m_timerQueue->add_timer(cb, Timestamp::now().add_time(interval), interval);
}

void EventLoop::cancel(TimerId timerId){
  m_timerQueue->cancel_timer(timerId);
}

void EventLoop::update_channel(Channel * channel){
  assert(channel->get_owner_loop() == this);
  assert_in_loop_thread();

  m_poller->update_channel(channel);
}

void EventLoop::remove_channel(Channel * channel){
  assert(channel->get_owner_loop() == this);
  assert_in_loop_thread();

  if(m_eventHandling){    // 为什么要有这样的断言
    assert(m_currentActiveChannel == channel || 
        std::find(m_activeChannels.begin(), m_activeChannels.end(), channel) == m_activeChannels.end());
  }

  m_poller->remove_channel(channel);
}

bool EventLoop::has_channel(Channel* channel){
  assert(channel->get_owner_loop() == this);
  assert_in_loop_thread();

  return m_poller->has_channel(channel);
}

void EventLoop::wakeup(){
  uint64_t one = 1;
  ssize_t n = ::write(m_eventfd, &one, sizeof one);
  if (n != sizeof one){
    LOG_SYSERR("EventLoop::wakeup() writes %d bytes instead of 8", n);
  }
}

void EventLoop::handle_read(){
  uint64_t one;
  ssize_t n = ::read(m_eventfd, &one, sizeof one);
  if (n != sizeof one){
    LOG_SYSERR("EventLoop::handle_read() reads %d bytes instead of 8", n);
  }
}

void EventLoop::handle_pending_functors(){
  std::vector<Functor> functors;
  m_callingPendingFunctors = true;

  {
    std::unique_lock<std::mutex> ul(m_mutex);
    functors.swap(m_pendingFunctors);
  }

  for(auto& pendingFunctor : functors){
    pendingFunctor();
  }

  m_callingPendingFunctors = false;
}