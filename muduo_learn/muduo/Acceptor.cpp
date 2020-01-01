#include <fcntl.h>  // open
#include <errno.h>  // errno

#include <tools/log/log.h>
#include <tools/base/Exception.h>
#include <tools/socket/ConnSocket.h>
#include <tools/socket/SocketsOps.h>

#include "muduo/Acceptor.h"
#include "muduo/EventLoop.h"

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr) : 
  m_loop(loop),
  m_acceptSocket(listenAddr),
  m_acceptChannel(loop, m_acceptSocket.get_sockfd()),
  m_listening(false),
  m_idleFd(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
  assert(m_idleFd >= 0);
  m_acceptSocket.set_reuse_address(true);
  
  m_acceptChannel.set_read_callback(std::bind(&Acceptor::handle_read, this));
}

Acceptor::~Acceptor(){
  m_acceptChannel.disable_all();
  m_acceptChannel.remove();
  sockets::close(m_idleFd);
}

void Acceptor::listen(){
  m_loop->assert_in_loop_thread();

  m_listening = true;
  m_acceptSocket.listen();
  m_acceptChannel.enable_reading();
}

void Acceptor::handle_read(){
  m_loop->assert_in_loop_thread();

  try{
    ConnSocket connSocket = m_acceptSocket.accept_nonblocking();
    if(m_newConnectionCallback){
      m_newConnectionCallback(connSocket, connSocket.get_remote_address());
    }
  }
  catch(const Exception &e){
    if(errno == EMFILE){
      sockets::close(m_idleFd);
      {
        ConnSocket connSocket = m_acceptSocket.accept_nonblocking();
      }
      m_idleFd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
    }
    else{
      LOG_SYSERR("Failed to accept in Acceptor::hand_read()");
    }
  }
}