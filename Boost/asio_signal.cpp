#include <iostream>
#include <functional>

#include <signal.h>

#include <boost/asio.hpp>               // io_service
#include <boost/system/error_code.hpp>  // error_code

using namespace boost::asio;

using error_code = boost::system::error_code;

void handler_sigint(const error_code & ec, int signo){
    if(ec){
        std::cout << ec.message() << std::endl;
        return;
    }

    std::cout << "receive a signal" << std::endl;

    if(signo != SIGINT){                            // 该信号处理函数只处理SIGINT信号
        return;
    }

    std::cout << "handler_sigint(): recevice signal: " << signo << std::endl;
    std::cout << "do something" << std::endl;
}

int main(){
    io_service io;                          // asio程序中必须要有io_service对象
    signal_set sig(io, SIGINT, SIGHUP);    // 创建signal_set对象，传入SIGINT、SIGUSR1对象

    // sig.add(SIGINT);
    // sig.add(SIGUSR1);
    sig.add(SIGALRM);

    auto handler_sighup = [](const error_code & ec, int signo){
        if(ec){
            std::cout << ec.message() << std::endl;
            return;
        }

        std::cout << "receive a signal" << std::endl;

        if(signo != SIGHUP){                            // 该信号处理函数只处理SIGUSR1信号
            return;
        }

        std::cout << "handler_sighup(): recevice signal: " << signo << std::endl;
        std::cout << "do something" << std::endl;
    };

    std::function<void(const error_code & ec, int signo)> handler_sigalrm = [](const error_code & ec, int signo){
        if(ec){
            std::cout << ec.message() << std::endl;
            return;
        }

        std::cout << "receive a signal" << std::endl;

        if(signo != SIGALRM){                            // 该信号处理函数只处理SIGUSR2信号
            return;
        }

        std::cout << "handler_sigalrm(): recevice signal: " << signo << std::endl;
        std::cout << "do something" << std::endl;
    };

    sig.async_wait(handler_sigint);
    sig.async_wait(handler_sighup);
    sig.async_wait(handler_sigalrm);

    io.run();

    std::cout << "io stoped" << std::endl;

    return 0;
}
