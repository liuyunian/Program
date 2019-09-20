#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <condition_variable>

class Server{
public:
    Server() = default;

    ~Server() = default;

    /**
     * 轮询方式
    void collectRequest(){
        while(true){
            std::cout << "收到一个请求：0" << std::endl;

            m_mutex.lock();
            requestList.push_back(0);
            m_mutex.unlock();

            std::chrono::milliseconds dura(50);
            std::this_thread::sleep_for(dura);
        }
    }

    void dealRequest(){
        while(true){
            m_mutex.lock();
            if(!requestList.empty()){
                std::cout << "处理请求：" << requestList.front() << std::endl;

                requestList.pop_front();
                m_mutex.unlock();
            }
            else{
                m_mutex.unlock();

                std::cout << "暂时没有请求" << std::endl;

                std::chrono::milliseconds dura(100);
                std::this_thread::sleep_for(dura);
            }
        }
    }
    */

    void collectRequest(){
        while(true){
            std::cout << "收到一个请求：0" << std::endl;

            m_mutex.lock();
            requestList.push_back(0);
            m_mutex.unlock();

            m_condv.notify_one();                           // 唤醒至少一个等待的线程

            std::chrono::milliseconds dura(50);
            std::this_thread::sleep_for(dura);
        }
    }

    void dealRequest(){
        while(true){
            std::unique_lock<std::mutex> ul(m_mutex);
            while(requestList.empty()){
                m_condv.wait(ul);
            }
                
            int request = requestList.front();
            requestList.pop_front();
            m_mutex.unlock();

            std::cout << "处理请求：" << request << std::endl;
        }
    }

private:
    std::list<int> requestList;
    std::mutex m_mutex;

    std::condition_variable m_condv; // 条件变量
};

int main(){
    Server server;

    std::thread collectRequestThread(&Server::collectRequest, &server);
    std::thread dealRequestThread(&Server::dealRequest, &server);

    collectRequestThread.join();
    dealRequestThread.join();

    return 0;
}

