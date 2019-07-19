#ifndef TIMER_H_
#define TIMER_H_

#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>
#include <functional>
#include <condition_variable>

class Timer{
public:
    Timer();

    ~Timer();

    /**
     * @brief 异步执行一次任务
     * @param msTime 延迟及间隔时间
     * @param fun 函数接口或lambda代码块
     * @param args 参数
     * @return true:已准备执行，否则失败
     */
    template<typename callable, typename... arguments>
    bool asyncOnce(int msTime, callable && fun, arguments &&... args) {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(fun), std::forward<arguments>(args)...));
        
        return startTimer(msTime, task, false);
    }
    
    /**
     * @brief 异步循环执行任务
     * @param msTime 延迟及间隔时间
     * @param fun 函数接口或lambda代码块
     * @param args 参数
     * @return true:已准备执行，否则失败
     */
    template<typename callable, typename... arguments>
    bool asyncLoop(int msTime, callable&& fun, arguments&&... args) {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(fun), std::forward<arguments>(args)...));
        
        return startTimer(msTime, task, true);
    }

    /**
     * @brief 取消定时器(若任务代码已执行则取消无效)
     */
    void cancelTimer();
    
private:
    /**
     * @brief 开始运行定时器
     * @param msTime 延迟运行(单位ms)
     * @param task 任务函数接口
     * @param bLoop 是否循环(默认执行1次)
     * @param async 是否异步(默认异步)
     * @return true:已准备执行，否则失败
     */
    bool startTimer(unsigned int msTime, std::function<void()> task, bool bLoop = false);

    /**
     * @brief 删除任务线程
     */
    void deleteThread();
    
private:
    std::atomic_bool m_bExpired;       //装载的任务是否已经过期
    std::atomic_bool m_bTryExpired;    //装备让已装载的任务过期(标记)
    std::atomic_bool m_bLoop;          //是否循环
    
    std::thread * m_thread;
    std::mutex m_thread_mutex;
    std::condition_variable_any m_condition;
};

#endif // TIMER_H_