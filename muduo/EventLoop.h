#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_

#include <stdlib.h>

#include <tools_cxx/noncopyable.h>
#include <tools_cxx/CurrentThread.h>

class EventLoop : noncopyable{
public:
    EventLoop();

    ~EventLoop();

    void loop();

    void assert_in_loopThread(){
        if(!is_in_loopThread()){
            abort_not_in_loopThread();
        }
    }

    bool is_in_loopThread(){
        return m_threadID == CurrentThread::get_tid();
    }

private:
    void abort_not_in_loopThread();

private:    
    bool m_looping;
    const int m_threadID;
};

#endif // EVENTLOOP_H_