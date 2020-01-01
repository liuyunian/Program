#ifndef ACCEPTOR_H_
#define ACCEPTOR_H_

#include <functional>

#include <tools/base/noncopyable.h>
#include <tools/socket/InetAddress.h>
#include <tools/socket/ServerSocket.h>

#include "muduo/Channel.h"

class EventLoop;
class ConnSocket;

class Acceptor : noncopyable {
public:
  typedef std::function<void(ConnSocket connSocket, const InetAddress&)> NewConnectionCallback;

  Acceptor(EventLoop *loop, const InetAddress &listenAddr);
  ~Acceptor();

  void set_connection_callback(const NewConnectionCallback &cb){
    m_newConnectionCallback = cb;
  }

  bool is_listening() const {
    return m_listening;
  }

  void listen();

private:
  void handle_read();

private:
  EventLoop *m_loop;
  ServerSocket m_acceptSocket;
  Channel m_acceptChannel;
  NewConnectionCallback m_newConnectionCallback;
  bool m_listening;
  int m_idleFd;
};

#endif // ACCEPTOR_H_