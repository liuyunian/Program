#include <iostream>

#include <unistd.h>

#include <muduo/Acceptor.h>
#include <muduo/EventLoop.h>
#include <tools/socket/ConnSocket.h>
#include <tools/socket/InetAddress.h>
#include <tools/socket/SocketsOps.h>

#define LISTEN_PORT 8080

void newConnection(ConnSocket socket, const InetAddress& peerAddr){
  std::cout << "newConnection(): accepted a new connection from " << peerAddr.to_ip_and_port().c_str() <<  std::endl;
  socket.write("How are you?\n", 13);
}

int main(){
  std::cout << "main(): pid =" <<  getpid() << std::endl;

  EventLoop loop;

  InetAddress listenAddr(LISTEN_PORT);
  Acceptor acceptor(&loop, listenAddr);
  acceptor.set_connection_callback(newConnection);
  acceptor.listen();

  loop.loop();
  return 0;
}

