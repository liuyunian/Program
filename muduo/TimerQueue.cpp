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
    
}

