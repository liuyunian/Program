#ifndef TIMERQUEUE_H_
#define TIMERQUEUE_H_

#include <set>
#include <atomic>

#include <tools_cxx/Timestamp.h>
#include <tools_cxx/noncopyable.h>

#include "Channel.h"
#include "TimerId.h"
#include "Timer.h"

class EventLoop;

class TimerQueue : noncopyable {
public:
    TimerQueue(EventLoop * loop);

    ~TimerQueue();

    TimerId add_timer(TimerCallback cb, Timestamp when, double interval);

    void cancel(TimerId timerId);

private:
    typedef std::pair<Timestamp, Timer *> TimerEntry;
    typedef std::set<TimerEntry> TimerSet;

    typedef std::pair<Timer *, int64_t> ActiveTimerEntry;
    typedef std::set<ActiveTimerEntry> ActiveTimerSet;

    void add_timer_inLoop(Timer * timer);

    void cancel_timer_inLoop(TimerId timerId);

    bool insert(Timer * timer);

    void handle_readEvent();                                                    // m_timerfd可读事件发生时的回调函数

    std::vector<TimerEntry> getExpired(Timestamp now);

    void reset(const std::vector<TimerEntry> & expired, Timestamp now);

private:
    EventLoop * m_loop;
    const int m_timerfd;
    Channel m_timerfdChannel;

    TimerSet m_timers;

    ActiveTimerSet m_activeTimers;
    ActiveTimerSet m_cancelingTimers;

    std::atomic_bool m_callingExpiredTimer;
};

#endif // TIMERQUEUE_H_