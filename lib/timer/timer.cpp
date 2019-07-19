#include <future>

#include "timer.h"

Timer::Timer() : 
    m_bExpired(true), 
    m_bTryExpired(false), 
    m_bLoop(false),
    m_thread(nullptr){}

Timer::~Timer(){
    m_bTryExpired = true;   //尝试使任务过期
    deleteThread();
}

bool Timer::startTimer(unsigned int msTime, std::function<void()> task, bool bLoop){
    if(!m_bExpired || m_bTryExpired){
        return false;  //任务未过期(即内部仍在存在或正在运行任务)
    }

    m_bExpired = false;
    m_bLoop = bLoop;
    
    deleteThread();
    m_thread = new std::thread([this, msTime, task](){
        while(!m_bTryExpired) {
            m_condition.wait_for(m_thread_mutex, std::chrono::milliseconds(msTime));  //休眠
            if (!m_bTryExpired) {
                task();     //执行任务
                
                if (!m_bLoop) {
                    break;
                }
            }
        }
        
        m_bExpired = true;      //任务执行完成(表示已有任务已过期)
        m_bTryExpired = false;  //为了下次再次装载任务
    });
    
    return true;
}

void Timer::cancelTimer(){
    if (m_bExpired || m_bTryExpired || !m_thread) {
        return;
    }
    
    m_bTryExpired = true;
    deleteThread();
}

void Timer::deleteThread(){
    if (m_thread) {
        m_condition.notify_one();   //休眠唤醒
        m_thread->join();           //等待线程退出
        delete m_thread;
        m_thread = nullptr;
    }
}