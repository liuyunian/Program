#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/steady_timer.hpp>

struct Interface{
    std::string interfaceName = "abc";
    bool trafficState = false;
    boost::asio::io_service io;
    boost::asio::steady_timer timer;

    Interface() : timer(io){}
};

// boost::asio::io_service io;
// // boost::asio::deadline_timer timer(io, boost::posix_time::seconds(1));
// boost::asio::steady_timer timer(io, std::chrono::milliseconds(500));

// void print(const boost::system::error_code& ec){
//     std::cout << "Hello, world!" << std::endl;

//     // timer.expires_at(timer.expires_at() + std::chrono::milliseconds(500));
//     timer.expires_from_now(std::chrono::milliseconds(500));
//     timer.async_wait(&print);
// }

void timerEvent(std::string interfaceName, const boost::system::error_code& ec){
    std::cout << interfaceName << std::endl;
}

void startTimer(Interface & interface, u_int64_t dura){
    interface.timer.expires_from_now(std::chrono::milliseconds(dura));
    interface.trafficState = true;

    interface.timer.async_wait(boost::bind(&timerEvent, interface.interfaceName, boost::asio::placeholders::error));
    interface.io.run();
}

int main(){
    // timer.async_wait(&print);

    // // std::cout << timer.expires_at() << std::endl;
    // // std::cout << timer.expires_from_now() << std::endl;

    // for(int i = 0; i < 10; ++ i){
    //     std::cout << i << std::endl;
    // }

    // io.run();
    Interface interface;
    startTimer(interface, 1000);

    return 0;
}

/**
 * g++ 编译
 *  g++ -o boost_timer.out boost_timer.cpp -lboost_system -lboost_thread 
 */