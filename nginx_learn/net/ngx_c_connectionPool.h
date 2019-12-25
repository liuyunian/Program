#ifndef NGX_C_CONNECTIONPOOL_H_
#define NGX_C_CONNECTIONPOOL_H_

#include <pthread.h> // pthread_mutex_t
#include <arpa/inet.h> // sockaddr_in

#include "_include/ngx_macro.h"

class Socket;
struct TCPConnection;
using ngx_event_handler = void (Socket::*)(TCPConnection *); // Socket成员函数指针

// 收包状态
enum RecvPktState{
    RECV_PKT_HEADER,
    RECV_PKT_BODY
};

// 当前连接的发送缓冲区状态
enum SendBufState{
    NGX_FREE,
    NGX_BUSY
};

/**
 * @brief 表示一个TCP连接
 */
struct TCPConnection{
    int sockfd = -1;                                // socket描述符
    unsigned validFlag:1 ;                          // 失效标志位
    u_int64_t curSeq = 0;                           // 判断连接是否过期
    
    struct sockaddr cliAddr;                     // 客户端的地址信息

    ngx_event_handler r_handler = nullptr;          // 读事件的处理方法
    ngx_event_handler w_handler = nullptr;          // 写事件的处理方法

    // 收包相关
    RecvPktState curRecvPktState = RECV_PKT_HEADER; // 记录当前通过该TCP连接的收包状态，初始为接收包头
    uint8_t pktHeader[20] = {0};                    // 用于存放包头数据，这里设置为固定的20字节，该值应该>=包头的实际长度
    uint8_t * recvPos = nullptr;                    // 指向当前要接收的数据
    uint16_t recvLen = 0;                           // 要接收数据的长度
    uint8_t * recvBuf = nullptr;                    // 接收缓冲区，实际包含消息头

    // 发包相关
    uint8_t * sendPos = nullptr;                      // 指向当前要发送的数据
    uint16_t sendLen = 0;                           // 要发送数据的长度
    uint8_t * sendBuf = nullptr;                    // 发送缓冲区，实际包含消息头
    SendBufState curSendBufState = NGX_FREE;        // 当前连接的发送缓冲区状态

    uint32_t eventType;                             // 记录监听的事件类型

    TCPConnection * next = nullptr; // 指向下一个TCP连接
};

class ConnectionPool{
public:
    ConnectionPool(int size);
    ~ConnectionPool();

    /**
     * @brief 从连接池中获取一个空闲连接
     * @param
     * @return
     */
    TCPConnection * ngx_get_connection(int sockfd);

    /**
     * @brief 释放连接并归还连接池
     */
    void ngx_free_connection(TCPConnection * c);

private:
    int m_poolSize; // 连接池的大小
    int m_freeSize; // 连接池中可用大小

    TCPConnection * m_connectionPool; // 连接池
    TCPConnection * m_free; // 指向第一个空闲的TCPConnection对象
    TCPConnection * m_tail; // 指向连接池最后一个TCPConnection对象
};

#endif // NGX_C_CONNECTIONPOOL_H_