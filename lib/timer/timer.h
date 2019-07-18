#ifndef TIMER_H_
#define TIMER_H_

#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>
#include <condition_variable>

class Timer{
public:
    Timer(const std::string sTimerName = "");   //构造定时器，附带名称
    ~Timer();

    /**
     * @brief 开始运行定时器
     * @param msTime 延迟运行(单位ms)
     * @param task 任务函数接口
     * @param bLoop 是否循环(默认执行1次)
     * @param async 是否异步(默认异步)
     * @return true:已准备执行，否则失败
     */
    bool startTimer(unsigned int msTime, std::function<void()> task, bool bLoop = false, bool async = true);
    
    /**
     * @brief 取消定时器，同步定时器无法取消(若任务代码已执行则取消无效)
     */
    void cancelTimer();
    
    /**
     * @brief 同步执行一次
     * @param msTime 延迟时间(ms)
     * @param fun 函数接口或lambda代码块
     * @param args 参数
     * @return true:已准备执行，否则失败
     */
    template<typename callable, typename... arguments>
    bool syncOnce(int msTime, callable && fun, arguments&&... args) {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(fun), std::forward<arguments>(args)...)); // 绑定任务函数或lambda成function
        return Start(msTime, task, false, false);
    }

    /**
     * @brief 异步执行一次任务
     * @param msTime 延迟及间隔时间
     * @param fun 函数接口或lambda代码块
     * @param args 参数
     * @return true:已准备执行，否则失败
     */
    template<typename callable, typename... arguments>
    bool AsyncOnce(int msTime, callable&& fun, arguments&&... args) {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(fun), std::forward<arguments>(args)...));
        
        return Start(msTime, task, false);
    }
    
    /**
     * @brief 异步执行一次任务(默认延迟10毫秒后执行)
     * @param fun 函数接口或lambda代码块
     * @param args 参数
     * @return true:已准备执行，否则失败
     */
    template<typename callable, typename... arguments>
    bool AsyncOnce(callable&& fun, arguments&&... args) {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(fun), std::forward<arguments>(args)...));
        
        return Start(10, task, false);
    }
    
    /**
     * @brief 异步循环执行任务
     * @param msTime 延迟及间隔时间
     * @param fun 函数接口或lambda代码块
     * @param args 参数
     * @return true:已准备执行，否则失败
     */
    template<typename callable, typename... arguments>
    bool AsyncLoop(int msTime, callable&& fun, arguments&&... args) {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(fun), std::forward<arguments>(args)...));
        
        return Start(msTime, task, true);
    }
    
    
private:
    void DeleteThread();    //删除任务线程
    
private:
    std::string m_sName;   // 定时器名称
    
    std::atomic_bool m_bExpired;       //装载的任务是否已经过期
    std::atomic_bool m_bTryExpired;    //装备让已装载的任务过期(标记)
    std::atomic_bool m_bLoop;          //是否循环
    
    std::thread * m_Thread = nullptr;
    std::mutex m_ThreadLock;
    std::condition_variable_any m_ThreadCon;
};

#endif // TIMER_H_