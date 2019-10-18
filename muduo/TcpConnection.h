#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_

#include <memory>
#include <functional>

#include <tools/base/noncopyable.h>
#include <tools/base/Timestamp.h>
#include <tools/socket/InetAddress.h>

#include "Callback.h"

// 使用前向声明的好处是：避免头文件过于庞大
class EventLoop; 
class Channel;
class Socket;

class TcpConnection : public noncopyable 
                      public std{
public:
    TcpConnection(const std::string& name, 
                  const InetAddress& localAddr, 
                  const InetAddress& peerAddr,
                  EventLoop* loop, 
                  Socket* socket);

    ~TcpConnection() = default;

    // get_xx set_xx is_xx
    EventLoop* get_eventLoop() const {
        return m_loop;
    }

    const std::string& get_name() const {
        return m_name;
    }

    const InetAddrss& get_localAddr() const {
        return m_localAddr;
    }

    const InetAddrss& get_peerAddr() const {
        return m_peerAddr;
    }

    bool is_connected() const {
        return m_state == Connected;
    }

    void set_connectionCallback(ConnectionCallback& cb){
        m_connectionCallback = cb;
    }

    void set_messageCallback(MessageCallback& cb){
        m_messageCallback = cb;
    }



private:
    enum State {
        Connecting,
        Connected
    };

    void handle_read(Timestamp time);

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
};

#endif // TCPCONNECTION_H_