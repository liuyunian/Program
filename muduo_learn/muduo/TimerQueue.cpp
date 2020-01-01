#include <iostream>

#include <string.h>         // memset
#include <assert.h>         // assert
#include <unistd.h>         // close
#include <sys/timerfd.h>    // timerfd_*

#include <tools/log/log.h>

#include "muduo/TimerQueue.h"
#include "muduo/EventLoop.h"
#include "muduo/Timer.h"

static int create_timerfd(){
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(timerfd < 0){
        LOG_SYSFATAL("Failed to create timerfd in create_timerfd()");
    }

    return timerfd;
}

static struct timespec get_time_from_now(Timestamp when){
    int64_t microseconds = when.get_microseconds_since_epoch() - Timestamp::now().get_microseconds_since_epoch();
    if (microseconds < 100){ // 间隔不足100us，按100us算
        microseconds = 100;
    }

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kmicrosecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kmicrosecondsPerSecond) * 1000);

    return ts;
}

static void reset_timerfd(int timerfd, Timestamp expiration){
    struct itimerspec newValue;
    struct itimerspec oldValue;
    memset(&newValue, 0, sizeof(newValue));
    memset(&oldValue, 0, sizeof(oldValue));

    newValue.it_value = get_time_from_now(expiration);
    if(::timerfd_settime(timerfd, 0, &newValue, &oldValue) < 0){
        LOG_SYSFATAL("Failed to reset timerfd in reset_timerfd()");
    }
}

static void read_timerfd(int timerfd, Timestamp now){
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
    if(n != sizeof(howmany)){
        LOG_SYSERR("TimerQueue::handle_event() reads %d bytes instead of 8", n);
    }
}

TimerQueue::TimerQueue(EventLoop* loop) : 
    m_loop(loop),
    m_timerfd(create_timerfd()),
    m_timerfdChannel(loop, m_timerfd),
    m_timers(),
    m_callingExpiredTimer(false)
{
    // 只有调用了timerfd_settime设置了时间之后，才会有事件触发，才会执行回调
    m_timerfdChannel.set_read_callback(std::bind(&TimerQueue::handle_event, this));
    m_timerfdChannel.enable_reading();
}

TimerQueue::~TimerQueue(){
    close(m_timerfd);
    
    for(auto & timerEntry : m_timers){
        delete timerEntry.second;
    }
}

TimerId TimerQueue::add_timer(TimerCallback cb, Timestamp when, double interval){
    Timer* timer = new Timer(cb, when, interval);
    m_loop->run_in_loop(std::bind(&TimerQueue::add_timer_in_loop, this, timer));

    return TimerId(timer, timer->get_sequence());
}

void TimerQueue::cancel_timer(TimerId timerId){
    m_loop->run_in_loop(std::bind(&TimerQueue::cancel_timer_in_loop, this, timerId));
}

void TimerQueue::add_timer_in_loop(Timer* timer){
    m_loop->assert_in_loop_thread();

    bool earliestChanged = insert(timer);
    if(earliestChanged){
        reset_timerfd(m_timerfd, timer->get_expiration());
    }
}

void TimerQueue::cancel_timer_in_loop(TimerId timerId){
    m_loop->assert_in_loop_thread();
    assert(m_timers.size() == m_activeTimers.size());

    ActiveTimerEntry entry(timerId.m_timer, timerId.m_sequence);
    auto iter = m_activeTimers.find(entry);
    if(iter != m_activeTimers.end()){
        size_t n = m_timers.erase(TimerEntry(iter->first->get_expiration(), iter->first));
        assert(n == 1);

        delete iter->first;
        m_activeTimers.erase(iter);
    }
    else if(m_callingExpiredTimer){
        m_cancelingTimers.insert(entry);
    }

    assert(m_timers.size() == m_activeTimers.size());
}

bool TimerQueue::insert(Timer* timer){
    m_loop->assert_in_loop_thread();
    assert(m_timers.size() == m_activeTimers.size());

    bool earliestChanged = false;
    Timestamp when = timer->get_expiration();
    auto iter = m_timers.begin();
    if(iter == m_timers.end() || when < iter->first){
        earliestChanged = true;
    }

    {
        std::pair<TimerSet::iterator, bool> result = m_timers.insert(TimerEntry(when, timer));
        assert(result.second);
    }

    {
        std::pair<ActiveTimerSet::iterator, bool> result = m_activeTimers.insert(ActiveTimerEntry(timer, timer->get_sequence()));
        assert(result.second);
    }

    assert(m_timers.size() == m_activeTimers.size());
    return earliestChanged;
}

void TimerQueue::handle_event(){
    m_loop->assert_in_loop_thread();
    Timestamp now(Timestamp::now());

    read_timerfd(m_timerfd, now);

    std::vector<TimerEntry> expiredTimers = get_expired(now);

    m_callingExpiredTimer = true;
    m_cancelingTimers.clear();
    for(auto& timerEntry : expiredTimers){
        timerEntry.second->run();
    }
    m_callingExpiredTimer = false;

    reset(expiredTimers, now);
}

std::vector<TimerQueue::TimerEntry> TimerQueue::get_expired(Timestamp now){
    assert(m_timers.size() == m_activeTimers.size());
    std::vector<TimerEntry> expiredTimers;

    TimerEntry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));      // 关键在于时间戳
    auto end = m_timers.lower_bound(sentry);
    assert(end == m_timers.end() || now < end->first);
    std::copy(m_timers.begin(), end, back_inserter(expiredTimers));
    m_timers.erase(m_timers.begin(), end);

    for(auto& timerEntry : expiredTimers){
        ActiveTimerEntry entry(timerEntry.second, timerEntry.second->get_sequence());
        size_t n = m_activeTimers.erase(entry);
        assert(n == 1);
    }

    assert(m_timers.size() == m_activeTimers.size());
    return expiredTimers;
}

void TimerQueue::reset(const std::vector<TimerEntry>& expiredTimers, Timestamp now){
    for(auto& timerEntry : expiredTimers){
        ActiveTimerEntry entry(timerEntry.second, timerEntry.second->get_sequence());
        if(timerEntry.second->is_repeated() && m_cancelingTimers.find(entry) == m_cancelingTimers.end()){
            timerEntry.second->restart(now);
            insert(timerEntry.second);
        }
        else{
            delete timerEntry.second;
        }
    }

    Timestamp newExpire;
    if(!m_timers.empty()){
        newExpire = m_timers.begin()->second->get_expiration();
    }

    if(newExpire.is_valid()){
        reset_timerfd(m_timerfd, newExpire);
    }
}
