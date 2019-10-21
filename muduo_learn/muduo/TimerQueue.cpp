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
    if (microseconds < 100){ // 以100us为最小单位
        microseconds = 100;
    }

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::k_microsecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::k_microsecondsPerSecond) * 1000);

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
    ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
    if(n != sizeof(howmany)){
        LOG_SYSERR("TimerQueue::handle_read_event() reads %d  bytes instead of 8", n);
    }
}

TimerQueue::TimerQueue(EventLoop* loop) : 
    m_loop(loop),
    m_timerfd(create_timerfd()),
    m_timerfdChannel(loop, m_timerfd),
    m_timers(),
    m_callingExpiredTimer(false)
{
    m_timerfdChannel.set_read_callback(std::bind(&TimerQueue::handle_read_event, this));
    m_timerfdChannel.enable_reading();
}

TimerQueue::~TimerQueue(){
    close(m_timerfd);
    
    for(auto & timerEntry : m_timers){
        delete timerEntry.second;
    }
}

TimerId TimerQueue::add_timer(TimerCallback cb, Timestamp when, double interval){
    Timer * timer = new Timer(cb, when, interval);
    m_loop->run_in_loop(std::bind(&TimerQueue::add_timer_in_loop, this, timer));

    return TimerId(timer, timer->get_sequence());
}

void TimerQueue::cancel(TimerId timerId){
    m_loop->run_in_loop(std::bind(&TimerQueue::cancel_timer_in_loop, this, timerId));
}

void TimerQueue::add_timer_in_loop(Timer * timer){
    m_loop->assert_in_loop_thread();

    bool earliestChanged = insert(timer);
    if(earliestChanged){
        reset_timerfd(m_timerfd, timer->get_expiration());
    }
}

void TimerQueue::cancel_timer_in_loop(TimerId timerId){
    m_loop->assert_in_loop_thread();
    assert(m_timers.size() == m_activeTimers.size());

    ActiveTimerEntry aTimerEntry(timerId.m_timer, timerId.m_sequence);
    auto iter = m_activeTimers.find(aTimerEntry);
    if(iter != m_activeTimers.end()){
        size_t n = m_timers.erase(TimerEntry(iter->first->get_expiration(), iter->first));
        assert(n == 1); (void)n;

        delete iter->first;
        m_activeTimers.erase(iter);
    }
    else if(m_callingExpiredTimer){
        m_cancelingTimers.insert(aTimerEntry);
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
        std::pair<TimerSet::iterator, bool> result
            = m_timers.insert(TimerEntry(when, timer));                             // 插入到m_timers中
        assert(result.second); (void)result;
    }

    {
        std::pair<ActiveTimerSet::iterator, bool> result
            = m_activeTimers.insert(ActiveTimerEntry(timer, timer->get_sequence()));    // 插入到m_activeTimers中
        assert(result.second); (void)result;
    }

    assert(m_timers.size() == m_activeTimers.size());
    return earliestChanged;
}

void TimerQueue::handle_read_event(){
    m_loop->assert_in_loop_thread();
    Timestamp now(Timestamp::now());

    read_timerfd(m_timerfd, now);           // 读取时间，避免一直触发

    std::vector<TimerEntry> expiredTimers = get_expired(now);

    m_callingExpiredTimer = true;
    m_cancelingTimers.clear();
    for(auto & timerEntry : expiredTimers){
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

    for(auto & timerEntry : expiredTimers){
        ActiveTimerEntry aTimerEntry(timerEntry.second, timerEntry.second->get_sequence());
        size_t n = m_activeTimers.erase(aTimerEntry);
        assert(n == 1); (void)n;
    }

    assert(m_timers.size() == m_activeTimers.size());
    return expiredTimers;
}

void TimerQueue::reset(const std::vector<TimerEntry>& expiredTimers, Timestamp now){
    for(auto & timerEntry : expiredTimers){
        ActiveTimerEntry aTimerEntry(timerEntry.second, timerEntry.second->get_sequence());
        if(timerEntry.second->is_repeated() && m_cancelingTimers.find(aTimerEntry) == m_cancelingTimers.end()){
            timerEntry.second->restart(now);
            insert(timerEntry.second);          // 还没启动定时事件，所以不用判断返回值
        }
        else{                                   // 一次性定时器或者已被取消的定时器是不能重置的，所以要删除
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
