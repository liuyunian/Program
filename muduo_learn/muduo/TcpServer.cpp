#include <functional>

#include <stdio.h>  // snprintf
#include <string.h> // strlen
#include <assert.h> // assert

#include <tools/log/log.h>
#include <tools/socket/SocketsOps.h>

#include "muduo/TcpServer.h"
#include "muduo/EventLoop.h"
#include "muduo/Acceptor.h"
#include "muduo/TcpConnection.h"
#include "muduo/EventLoopThreadPool.h"

TcpServer::TcpServer(const std::string& name, const InetAddress& listenAddr, EventLoop* loop, Option option) : 
    m_name(name),
    m_ipPort(listenAddr.to_ip_and_port()),
    m_started(false),
    m_loop(CHECK_NOT_NULLPTR(loop)),
    m_acceptor(new Acceptor(loop, listenAddr, option == ReusePort)),
    m_threadPool(new EventLoopThreadPool(m_loop)),
    m_connectionCallback(default_connection_callback),
    m_messageCallback(default_message_callback),
    m_nextConnId(1)
{
    m_acceptor->set_new_connection_callback(std::bind(&TcpServer::new_connection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer(){
    m_loop->assert_in_loop_thread();
    LOG_DEBUG("TcpServer::~TcpServer() [%s] destructing", m_name.c_str());
}

void TcpServer::set_number_of_threads(int numThreads){
    assert(numThreads >= 0);
    m_threadPool->set_number_of_threads(numThreads);
}

void TcpServer::start(){
    if(!m_started){
        m_started = true;

        m_threadPool->start();

        assert(!m_acceptor->is_listening());
        m_loop->run_in_loop(std::bind(&Acceptor::listen, m_acceptor.get()));
    }
}

void TcpServer::new_connection(Socket* socket, const InetAddress& peerAddr){
    m_loop->assert_in_loop_thread();
    EventLoop* ioLoop = m_threadPool->get_next_loop();

    char buf[64];
    snprintf(buf, strlen(buf), "-%s#%d", m_ipPort.c_str(), m_nextConnId);
    ++ m_nextConnId;
    std::string connName = m_name + buf;
    LOG_INFO("TcpServer [%s] new connection [%s] from %s", m_name.c_str(), connName.c_str(), peerAddr.to_ip_and_port().c_str());

    InetAddress localAddr(sockets::get_local_addr(socket->get_sockfd()));
    TcpConnectionPtr conn(new TcpConnection(connName, localAddr, peerAddr, ioLoop, socket));
    m_connections[connName] = conn;

    conn->set_connection_callback(m_connectionCallback);
    conn->set_message_callback(m_messageCallback);
    conn->set_close_callback(std::bind(&TcpServer::remove_connection, this, std::placeholders::_1));

    ioLoop->run_in_loop(std::bind(&TcpConnection::connect_established, conn));
}

void TcpServer::remove_connection(const TcpConnectionPtr& conn){
    m_loop->run_in_loop([this, &conn](){
        m_loop->assert_in_loop_thread();
        LOG_INFO("TcpServer::removeConnectionInLoop [%s] - connection", conn->get_name().c_str());

        size_t n = m_connections.erase(conn->get_name());
        (void)n;
        assert(n == 1);

        EventLoop* ioLoop = conn->get_loop();
        ioLoop->queue_in_loop(std::bind(&TcpConnection::connect_destroyed, conn));
    });
}