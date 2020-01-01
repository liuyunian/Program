#include "Timer.h"

std::atomic<int64_t> Timer::m_sNumCreated;

Timer::Timer(TimerCallback& cb, Timestamp when, double interval) : 
    m_callback(std::move(cb)),
    m_expiration(when),
    m_interval(interval),
    m_repeat(interval > 0.0),
    m_sequence(++ m_sNumCreated)
{}

void Timer::restart(Timestamp now){
    if(m_repeat){
        m_expiration = now.add_time(m_interval);
    }
    else{
        m_expiration = Timestamp::invalid();
    }
}