#include <iostream>
#include <functional>

#include <unistd.h>

#include <muduo/EventLoop.h>
#include <muduo/TCPServer.h>
#include <muduo/Buffer.h>
#include <muduo/TCPConnection.h>

#include <tools/log/log.h>
#include <tools/base/Timestamp.h>
#include <tools/socket/InetAddress.h>

#define LISTEN_PORT 9000

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
  void onConnection(const TCPConnectionPtr& tcpc){
    if(tcpc->is_connected()){
      LOG_INFO("onConnection(): new connection [%s] from %s", 
                tcpc->get_name().c_str(), 
                tcpc->get_peer_addr().to_ip_and_port().c_str()
              );
    }
    else{
      LOG_INFO("onConnection(): new connection [%s] is down", tcpc->get_name().c_str());
    }
  }

  void onMessage(const TCPConnectionPtr &tcpc, Buffer *buf, Timestamp recvTime){
    LOG_INFO("onMessage(): received %d bytes data from connection [%s] at %s", buf->readable_bytes(), tcpc->get_name().c_str(), recvTime.to_formatted_string().c_str());
    tcpc->send(buf->retrieve_all_as_string());
  }

private:
  TCPServer m_server;
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