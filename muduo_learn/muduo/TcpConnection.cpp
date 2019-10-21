#include <assert.h> // assert
#include <errno.h>  // errno

#include <tools/log/log.h>
#include <tools/socket/Socket.h>
#include <tools/socket/SocketsOps.h>

#include "muduo/TcpConnection.h"
#include "muduo/EventLoop.h"
#include "muduo/Channel.h"

void default_connection_callback(const TcpConnectionPtr& conn){

}

void default_message_callback(const TcpConnectionPtr& conn, const char* data, ssize_t len){

}

TcpConnection::TcpConnection(const std::string& name, 
                            const InetAddress& localAddr, 
                            const InetAddress& peerAddr,
                            EventLoop* loop, 
                            Socket* socket) : 
    m_name(name),
    m_state(Connecting),
    m_localAddr(localAddr),
    m_peerAddr(peerAddr),
    m_loop(CHECK_NOT_NULLPTR(loop)),
    m_socket(socket),
    m_channel(new Channel(m_loop, m_socket->get_sockfd()))
{
    m_channel->set_read_callback(std::bind(&TcpConnection::handle_read, this, std::placeholders::_1));
    m_channel->set_close_callback(std::bind(&TcpConnection::handle_close, this));
    m_channel->set_error_callback(std::bind(&TcpConnection::handle_error, this));
    m_socket->set_keep_alive(true);
}

void TcpConnection::connect_established(){
    m_loop->assert_in_loop_thread();
    assert(m_state == Connecting);
    m_state = Connected;

    m_channel->tie(shared_from_this()); // 将channel与channel持有者TcpConnection绑定
    m_channel->enable_reading();

    m_connectionCallback(shared_from_this());
}

void TcpConnection::connect_destroyed(){
    m_loop->assert_in_loop_thread();

    if(m_state == Connected){
        m_state = Disconnected;
        m_channel->disable_all();

        m_connectionCallback(shared_from_this());
    }

    m_channel->remove();
}

void TcpConnection::handle_read(Timestamp time){
    m_loop->assert_in_loop_thread();

    int savedErrno;
    char buf[65536];
    ssize_t n = m_socket->read(buf, sizeof(buf));
    if(n > 0){
        m_messageCallback(shared_from_this(), buf, n);
    }
    else if(n == 0){
        handle_close();
    }
    else{
        savedErrno = errno;
        LOG_SYSFATAL("TcpConnection::handle_read");
        handle_error();
    }
}

void TcpConnection::handle_close(){
    m_loop->assert_in_loop_thread();

    assert(m_state == Connected || m_state == Disconnecting);
    m_state = Disconnected;
    m_channel->disable_all();

    TcpConnectionPtr guardThis(shared_from_this());
    m_connectionCallback(guardThis);
    // must be the last line
    m_closeCallback(guardThis);
}

void TcpConnection::handle_error(){
    LOG_ERR("TcpConnection::handle_error()");
}

