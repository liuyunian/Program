#include <iostream>
#include <mutex>
#include <thread>
#include <list>

class Server{
private:
    std::list<int> requestList;
    std::mutex s_mutex;

public:
    void dealRequest(){
        for(int i = 1; i <= 400; i++){
            s_mutex.lock();

            std::chrono::milliseconds dura(50);
            std::this_thread::sleep_for(dura);

            if(!requestList.empty()){
                std::cout << "处理请求：" << requestList.front() << std::endl;
                requestList.pop_front();
                s_mutex.unlock();
            }
            else{
                s_mutex.unlock();
                std::cout << "暂时没有请求" << i << std::endl;
            }
        }
    }

    void collectRequest(){
        for(int i = 0; i < 1000; i++){
            std::chrono::milliseconds dura(50);
            std::this_thread::sleep_for(dura);
            std::cout << "收到一个请求：" << i << std::endl;
            std::unique_lock<std::mutex> ul(s_mutex, std::defer_lock);
            if(ul.try_lock()){
                requestList.push_back(i);
            }
            else{
                std::cout << "等待解锁" << std::endl;
            }
        }
    }
};

int main(){
    Server server;

    std::thread dealThread1(&Server::dealRequest, &server);
    std::thread collectThread(&Server::collectRequest, &server);
    collectThread.join();
    dealThread1.join();

    return 0;
}