#ifndef TIMERQUEUE_H_
#define TIMERQUEUE_H_

#include <set>
#include <vector>
#include <atomic>
#include <functional>

#include <tools/base/Timestamp.h>
#include <tools/base/noncopyable.h>

#include "muduo/Channel.h"
#include "muduo/TimerId.h"
#include "muduo/Callbacks.h"

class EventLoop;
class Timer;

class TimerQueue : noncopyable {
public:
    TimerQueue(EventLoop* loop);

    ~TimerQueue();

    TimerId add_timer(TimerCallback cb, Timestamp when, double interval);

    void cancel(TimerId timerId);

private:
    typedef std::pair<Timestamp, Timer*> TimerEntry;
    typedef std::set<TimerEntry> TimerSet;

    typedef std::pair<Timer*, int64_t> ActiveTimerEntry;
    typedef std::set<ActiveTimerEntry> ActiveTimerSet;

    void add_timer_in_loop(Timer* timer);

    void cancel_timer_in_loop(TimerId timerId);

    bool insert(Timer* timer);

    void handle_read_event();                                                    // m_timerfd可读事件发生时的回调函数

    std::vector<TimerEntry> get_expired(Timestamp now);

    void reset(const std::vector<TimerEntry>& expiredTimers, Timestamp now);

private:
    EventLoop* m_loop;
    const int m_timerfd;
    Channel m_timerfdChannel;

    TimerSet m_timers;

    ActiveTimerSet m_activeTimers;
    ActiveTimerSet m_cancelingTimers;

    std::atomic_bool m_callingExpiredTimer;
};

#endif // TIMERQUEUE_H_