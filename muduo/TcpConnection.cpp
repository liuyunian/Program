#include <tools/socket/Socket.h>

#include "TcpConnection.h"
#include "EventLoop.h"
#include "Channel"

TcpConnection::TcpConnection(const std::string& name, 
                            const InetAddress& localAddr, 
                            const InetAddress& peerAddr,
                            EventLoop* loop, 
                            Socket* socket);
    m_name(name),
    m_state(Connecting),
    m_localAddr(localAddr),
    m_peerAddr(peerAddr),
    m_loop(loop),
    m_socket(socket),
    m_channel(m_loop, m_socket->get_sockfd())
{
    m_channel->set_readCallback(&TcpConnection::handleRead, this, std::placeholders::_1);
    m_socket->set_keepAlive(true);
}

void TcpConnection::handle_read(Timestamp time){
    m_loop->assert_in_loopThread();
}


TcpConnection::~TcpConnection();

