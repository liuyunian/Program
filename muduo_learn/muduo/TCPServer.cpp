#include <functional>

#include <stdio.h>  // snprintf
#include <string.h> // strlen
#include <assert.h> // assert

#include <tools/log/log.h>
#include <tools/socket/SocketsOps.h>

#include "muduo/TCPServer.h"
#include "muduo/EventLoop.h"
#include "muduo/Acceptor.h"
#include "muduo/TCPConnection.h"
#include "muduo/EventLoopThreadPool.h"

TCPServer::TCPServer(const std::string &name, const InetAddress &listenAddr, EventLoop *loop) : 
  m_name(name),
  m_ipPort(listenAddr.to_ip_and_port()),
  m_started(false),
  m_loop(loop),
  m_acceptor(new Acceptor(loop, listenAddr)),
  m_threadPool(new EventLoopThreadPool(m_loop)),
  m_nextConnId(1)
{
  m_acceptor->set_connection_callback(std::bind(&TCPServer::new_connection, this, std::placeholders::_1, std::placeholders::_2));
}

TCPServer::~TCPServer(){
    m_loop->assert_in_loop_thread();
    LOG_DEBUG("TCPServer::~TCPServer() [%s] destructing", m_name.c_str());
}

void TCPServer::set_number_of_threads(int numThreads){
    assert(numThreads >= 0);
    m_threadPool->set_number_of_threads(numThreads);
}

void TCPServer::start(){
    if(!m_started){
        m_started = true;

        m_threadPool->start();

        assert(!m_acceptor->is_listening());
        m_loop->run_in_loop(std::bind(&Acceptor::listen, m_acceptor.get()));
    }
}

void TCPServer::new_connection(ConnSocket connSocket, const InetAddress &peerAddr){
    m_loop->assert_in_loop_thread();
    EventLoop* ioLoop = m_threadPool->get_next_loop();

    char buf[64];
    snprintf(buf, strlen(buf), "-%s#%d", m_ipPort.c_str(), m_nextConnId);
    ++ m_nextConnId;
    std::string connName = m_name + buf;
    LOG_INFO("TCPServer [%s] new connection [%s] from %s", m_name.c_str(), connName.c_str(), peerAddr.to_ip_and_port().c_str());

    InetAddress localAddr(sockets::get_local_addr(connSocket.get_sockfd()));
    TCPConnectionPtr tcpc(new TCPConnection(connName, localAddr, peerAddr, ioLoop, connSocket));
    m_connections[connName] = tcpc;

    tcpc->set_connection_callback(m_connectionCallback);
    tcpc->set_message_callback(m_messageCallback);
    tcpc->set_close_callback(std::bind(&TCPServer::remove_connection, this, std::placeholders::_1));

    ioLoop->run_in_loop(std::bind(&TCPConnection::connect_established, tcpc));
}

void TCPServer::remove_connection(const TCPConnectionPtr& tcpc){
  m_loop->run_in_loop([this, &tcpc](){
    m_loop->assert_in_loop_thread();
    LOG_INFO("TCPServer::removeConnectionInLoop [%s] - connection", tcpc->get_name().c_str());

    size_t n = m_connections.erase(tcpc->get_name());
    (void)n;
    assert(n == 1);

    EventLoop *ioLoop = tcpc->get_loop();
    ioLoop->queue_in_loop(std::bind(&TCPConnection::connect_destroyed, tcpc));
  });
}