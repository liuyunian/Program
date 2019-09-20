#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <condition_variable>

class Server{
public:
    Server() = default;

    ~Server() = default;

    void collectRequest(){
        while(true){
            std::cout << "收到一个请求：0" << std::endl;

            m_mutex.lock();
            requestList.push_back(0);
            m_mutex.unlock();

            m_condv_any.notify_one();                           // 唤醒至少一个等待的线程

            std::chrono::milliseconds dura(50);
            std::this_thread::sleep_for(dura);
        }
    }

    void dealRequest(){
        while(true){
            // std::unique_lock<std::mutex> ul(m_mutex);
            while(requestList.empty()){
                // m_condv_any.wait(ul);
                m_condv_any.wait(m_mutex);
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

    std::condition_variable_any m_condv_any; // 条件变量
};

int main(){
    Server server;

    std::thread collectRequestThread(&Server::collectRequest, &server);
    std::thread dealRequestThread(&Server::dealRequest, &server);

    collectRequestThread.join();
    dealRequestThread.join();

    return 0;
}

