#include <string.h>
#include <unistd.h>         // close
#include <sys/timerfd.h>    // timerfd_*

#include <tools_cxx/log.h>

#include "TimerQueue.h"

static int create_timerfd(){
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(timerfd < 0){
        LOG_SYSFATAL("Failed to create timerfd in create_timerfd()");
    }

    return timerfd;
}

static struct timespec get_time_from_now(Timestamp when){
    int64_t microseconds = when.get_microSecondsSinceEpoch() - Timestamp::now().get_microSecondsSinceEpoch();
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
    if(timerfd_settime(timerfd, 0, &newValue, &oldValue) < 0){
        LOG_SYSFATAL("Failed to reset timerfd in reset_timerfd()");
    }
}

TimerQueue::TimerQueue(EventLoop * loop) : 
    m_loop(loop),
    m_timerfd(create_timerfd()),
    m_timerfdChannel(loop, m_timerfd),
    m_timers(),
    m_callingExpiredTimer(false)
{
    m_timerfdChannel.set_readCallback(std::bind(&TimerQueue::handle_readEvent, this));
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
    add_timer_inLoop(timer);

    return TimerId(timer, timer->get_sequence());
}

void TimerQueue::cancel(TimerId timerId){
    cancel_timer_inLoop(timerId);
}

void TimerQueue::add_timer_inLoop(Timer * timer){
    m_loop->assert_in_loopThread();

    bool earliestChanged = insert(timer);
    if(earliestChanged){
        reset_Timerfd(m_timerfd, timer->get_expiration());
    }
}

void TimerQueue::cancel_timer_inLoop(TimerId timerId){
    m_loop->assert_in_loopThread();
    assert(m_timers.size() == m_activeTimers.size());

    ActiveTimerEntry aTimerEntry(TimerId.m_timer, TimerId.m_sequence);
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

bool TimerQueue::insert(Timer * timer){
    m_loop->assert_in_loopThread();
    assert(m_timers.size() == m_activeTimers.size());

    bool earliestChanged = false;
    Timestamp when = timer->get_expiration();
    auto iter = m_timers.begin();
    if(iter == m_timers.end() || when < iter->first){
        earliestChanged = true;
    }

    {
        std::pair<TimerSet::iterator, bool> result
            = m_timers.insert(TimerEntry(when, timer));                             // 插入到timers_中
        assert(result.second); (void)result;
    }

    {
        std::pair<ActiveTimerSet::iterator, bool> result
            = m_activeTimers.insert(ActiveTimerEntry(timer, timer->sequence()));    // 插入到activeTimers_中
        assert(result.second); (void)result;
    }

    assert(m_timers.size() == m_activeTimers.size());
    return earliestChanged;
}

void TimerQueue::handle_readEvent(){

}

std::vector<TimerEntry> TimerQueue::getExpired(Timestamp now){
    
}

void TimerQueue::reset(const std::vector<TimerEntry> & expired, Timestamp now){

}
