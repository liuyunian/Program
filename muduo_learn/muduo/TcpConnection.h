#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_

#include <memory>
#include <functional>

#include <tools/base/noncopyable.h>
#include <tools/base/Timestamp.h>
#include <tools/socket/InetAddress.h>

#include "muduo/Callbacks.h"

void default_connection_callback(const TcpConnectionPtr& conn);

void default_message_callback(const TcpConnectionPtr& conn, const char* data, ssize_t len);

// 使用前向声明的好处是：避免头文件过于庞大
class EventLoop; 
class Channel;
class Socket;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>, 
                      noncopyable{
public:
    TcpConnection(const std::string& name, 
                  const InetAddress& localAddr, 
                  const InetAddress& peerAddr,
                  EventLoop* loop, 
                  Socket* socket);

    ~TcpConnection() = default;

    // get_xx set_xx is_xx
    EventLoop* get_loop() const {
        return m_loop;
    }

    const std::string& get_name() const {
        return m_name;
    }

    const InetAddress& get_local_addr() const {
        return m_localAddr;
    }

    const InetAddress& get_peer_addr() const {
        return m_peerAddr;
    }

    bool is_connected() const {
        return m_state == Connected;
    }

    void set_connection_callback(const ConnectionCallback& cb){
        m_connectionCallback = cb;
    }

    void set_message_callback(const MessageCallback& cb){
        m_messageCallback = cb;
    }

    void set_close_callback(const CloseCallback& cb){
        m_closeCallback = cb;
    }

    // other
    void connect_established();

    void connect_destroyed();

private:
    enum State {
        Connecting,
        Connected,
        Disconnecting,
        Disconnected
    };

    void handle_read(Timestamp time);

    void handle_close();

    void handle_error();

private:
    std::string m_name;         // 连接名
    State m_state;              // 记录当前TCP连接的状态
    InetAddress m_localAddr;    // 连接的本地端地址
    InetAddress m_peerAddr;     // 对端地址

    EventLoop* m_loop;                  // 记录所属的EventLoop
    std::unique_ptr<Socket> m_socket;   // 连接对应的socket
    std::unique_ptr<Channel> m_channel; // 对应的事件channel

    ConnectionCallback m_connectionCallback;
    MessageCallback m_messageCallback;
    CloseCallback m_closeCallback;
};

#endif // TCPCONNECTION_H_