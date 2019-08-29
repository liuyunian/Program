#include <iostream>     // cout ... 

#include <unistd.h>     // sleep

#include "thread_oriented.h"

class ThisThread : public Thread {
public:
    ThisThread(int count) : 
        m_count(count)
    {
        std::cout << "ThisThread() ..." << std::endl;
    }

    ~ThisThread(){
        std::cout << "~ThisThread() ..." << std::endl;
    }

private:
    void run(){
        while(m_count > 0){
            std::cout << "this is a thread" << std::endl;
            
            sleep(1);

            -- m_count;
        }
    }

private:
    int m_count;
};


int main(){
    ThisThread t(5);
    t.start();

    t.join();

    return 0;
}