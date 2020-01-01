#include <assert.h> // assert
#include <errno.h>  // errno

#include <tools/log/log.h>
#include <tools/socket/ConnSocket.h>
#include <tools/socket/SocketsOps.h>

#include "muduo/TCPConnection.h"
#include "muduo/EventLoop.h"
#include "muduo/Channel.h"

TCPConnection::TCPConnection(const std::string &name, 
                            const InetAddress &localAddr, 
                            const InetAddress &peerAddr,
                            EventLoop *loop, 
                            ConnSocket connSocket) : 
  m_name(name),
  m_state(Connecting),
  m_localAddr(localAddr),
  m_peerAddr(peerAddr),
  m_loop(loop),
  m_connSocket(connSocket),
  m_channel(new Channel(m_loop, m_connSocket.get_sockfd()))
{
  m_channel->set_read_callback(std::bind(&TCPConnection::handle_read, this, std::placeholders::_1));
  m_channel->set_close_callback(std::bind(&TCPConnection::handle_close, this));
  m_channel->set_error_callback(std::bind(&TCPConnection::handle_error, this));
  m_connSocket.set_keep_alive(true);
}

void TCPConnection::connect_established(){
  m_loop->assert_in_loop_thread();
  assert(m_state == Connecting);
  m_state = Connected;

  m_channel->tie(shared_from_this()); // 将channel与channel持有者TCPConnection绑定
  m_channel->enable_reading();

  m_connectionCallback(shared_from_this());
}

void TCPConnection::connect_destroyed(){
  m_loop->assert_in_loop_thread();

  if(m_state == Connected){
    m_state = Disconnected;
    m_channel->disable_all();

    m_connectionCallback(shared_from_this());
  }

  m_channel->remove();
}

void TCPConnection::handle_read(Timestamp recvTime){
  m_loop->assert_in_loop_thread();

  char extrabuf[65536];
  struct iovec vec[2];
  const size_t writable = m_inputBuffer.writable_bytes();
  vec[0].iov_base = m_inputBuffer.writable_index();
  vec[0].iov_len = writable;
  vec[1].iov_base = extrabuf;
  vec[1].iov_len = sizeof extrabuf;
  const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
  const ssize_t len = m_connSocket.readv(vec, iovcnt);
  if(len < 0){
    LOG_SYSERR("TCPConnection::handle_read");
    handle_error();
  }
  else if(len == 0){
    handle_close();
  }
  else{
    if(static_cast<size_t>(len) <= writable){
      m_inputBuffer.adjust_writer_index(len);
    }
    else{
      m_inputBuffer.adjust_writer_index(writable);
      m_inputBuffer.append(extrabuf, len - writable);
    }

    m_messageCallback(shared_from_this(), &m_inputBuffer, recvTime);
  }
}

void TCPConnection::handle_close(){
  m_loop->assert_in_loop_thread();

  assert(m_state == Connected || m_state == Disconnecting);
  m_state = Disconnected;
  m_channel->disable_all();

  TCPConnectionPtr guardThis(shared_from_this());
  m_connectionCallback(guardThis);
  // must be the last line
  m_closeCallback(guardThis);
}

void TCPConnection::handle_error(){
  LOG_ERR("TCPConnection::handle_error()");
}

