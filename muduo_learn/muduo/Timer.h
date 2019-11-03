#ifndef TIMER_H_
#define TIMER_H_

#include <atomic>

#include <tools/base/Timestamp.h>
#include <tools/base/noncopyable.h>

#include "muduo/Callbacks.h"

class Timer : noncopyable {
public:
    Timer(TimerCallback& cb, Timestamp when, double interval);

    ~Timer() = default;

    Timestamp get_expiration() const {
        return m_expiration;
    }

    bool is_repeated() const {
        return m_repeat;
    }

    int64_t get_sequence() const {
        return m_sequence;
    }

    void run() const {
        m_callback();
    }

    void restart(Timestamp now);

    static int64_t get_timer_num(){
        return m_sNumCreated;
    }

private:
    const TimerCallback m_callback;
    Timestamp m_expiration;
    const double m_interval;
    const bool m_repeat;
    const int64_t m_sequence;

    static std::atomic<int64_t> m_sNumCreated;
};

#endif // TIMER_H_