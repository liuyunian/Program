#include <iostream>
#include <functional>

#include <unistd.h>

#include <muduo/EventLoop.h>
#include <muduo/TcpServer.h>
#include <tools/socket/InetAddress.h>

#define LISTEN_PORT 8080

class TestServer {
public:
    TestServer(const InetAddress& listenAddr, EventLoop* loop) : 
        m_server("TestServer", listenAddr, loop){}

    ~TestServer() = default;

    void start(){
        m_server.set_connection_callback(std::bind(&TestServer::onConnection, this, std::placeholders::_1));
        m_server.set_message_callback(std::bind(&TestServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        m_server.start();
    }

private:
    void onConnection(const TcpConnectionPtr& conn){
        std::cout << "onConnection(): pid = " << getpid() << std::endl;
        if(conn->is_connected()){
            std::cout << "onConnection(): new connection [" << conn->get_name() << "]" 
                    << " from " << conn->get_peer_addr().to_ip_and_port() 
                    << std::endl;
        }
        else{
            std::cout << "onConnection(): connection [" << conn->get_name() << "] is down" << std::endl;
        }
    }

    void onMessage(const TcpConnectionPtr& conn,  const char* data, ssize_t len){
        std::cout << "onMessage(): pid = " << getpid() << std::endl;
        std::cout << "onMessage(): received " << len << " bytes from connection [" << conn->get_name() << "]" << std::endl;
    }

private:
    TcpServer m_server;
};

int main(){
    std::cout << "main(): pid = " << getpid() << std::endl;

    InetAddress listenAddr(LISTEN_PORT);
    EventLoop loop;

    TestServer testServer(listenAddr, &loop);
    testServer.start();

    loop.loop();
    return 0;
}