#include <iostream>
#include <chrono>       // chrono::milliseconds
#include <functional>   // function

#include <boost/asio.hpp>               // io_service
#include <boost/bind.hpp>
#include <boost/asio/steady_timer.hpp>  // steady_timer
#include <boost/system/error_code.hpp>  // error_code

using namespace boost::asio;

using error_code = boost::system::error_code;

/**
 * 同步定时器
int main(){
    io_service io;
    steady_timer stimer(io, std::chrono::milliseconds(5000));
    std::cout << "timer: start" << std::endl;

    stimer.wait();

    std::cout << "timer: time out" << std::endl;

    return 0;
}
*/

/**
 * 异步定时器
void handler_timeout1(const error_code & ec){
    if(ec){
        std::cout << "error_code: " << ec.message() << std::endl;
        return;
    }

    std::cout << "handler_timeout1: time out" << std::endl;
}

int main(){
    io_service io;
    steady_timer stimer(io, std::chrono::milliseconds(5000));
    std::cout << "timer: start" << std::endl;

    stimer.async_wait(handler_timeout1);
    stimer.async_wait([](const error_code & ec){
        if(ec){
            std::cout << "error_code: " << ec.message() << std::endl;
            return;
        }

        std::cout << "handler_timeout2: time out" << std::endl;
    });

    std::function<void(const error_code & ec)> handler_timeout3 = [](const error_code & ec){
        if(ec){
            std::cout << "error_code: " << ec.message() << std::endl;
            return;
        }

        std::cout << "handler_timeout3: time out" << std::endl;
    };
    stimer.async_wait(handler_timeout3);

    // stimer.cancel_one();
    // stimer.cancel();

    io.run();

    std::cout << "io stoped" << std::endl;
    return 0;
}
*/

class Timer{
public:
    template<typename F>
    Timer(io_service & io, int count_max, F func) : 
        m_count(0),
        m_count_max(count_max),
        m_func(func),
        m_stimer(io, std::chrono::milliseconds(1000))
    {
        m_stimer.async_wait(boost::bind(&Timer::handler, this, boost::asio::placeholders::error));
    }

    ~Timer() = default;

private:
    void handler(const error_code & ec){
        if(ec){
            std::cout << "error_code: " << ec.message() << std::endl;
        }

        ++ m_count;
        if(m_count >= m_count_max){
            return;
        }

        m_func();

        m_stimer.expires_from_now(std::chrono::milliseconds(1000));
        m_stimer.async_wait(boost::bind(&Timer::handler, this, boost::asio::placeholders::error));

    }

private:
    int m_count;                    // 计数器
    int m_count_max;                // 计数器的最大值
    std::function<void()> m_func;   // 传入的函数
    steady_timer m_stimer;          // steady_timer定时器
};

int main(){
    io_service io;

    Timer t(io, 5, []{
        std::cout << "time out" << std::endl;
    });

    io.run();

    std::cout << "io stoped" << std::endl;

    return 0;
}