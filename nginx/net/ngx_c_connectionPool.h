#ifndef NGX_C_CONNECTIONPOOL_H_
#define NGX_C_CONNECTIONPOOL_H_

#include <arpa/inet.h> // sockaddr_in

class Socket;
struct TCPConnection;
using ngx_event_handler = void (Socket::*)(TCPConnection *); // Socket成员函数指针

/**
 * @brief 表示一个TCP连接
 */
struct TCPConnection{
    int sockfd = -1; // socket描述符
    unsigned instance:1 ; // 失效标志位
    u_int64_t curSeq = 0; // 判断连接是否过期
    
    struct sockaddr cliAddr; // 客户端的地址信息

    // uint8_t r_ready; // 读准备好标记--0：没准备好，1：准备好了
    uint8_t w_ready = 0; // 写准备好标记--0：没准备好，1：准备好了
    ngx_event_handler r_handler = nullptr; // 读事件的处理方法
    ngx_event_handler w_handler = nullptr; // 写事件的处理方法

    // 收包相关
    int curRecvPktState = INVALID_STATE; // 记录当前通过该TCP连接的收包状态，初始为无效状态
    uint8_t pktHeader[20] = {0}; // 用于存放包头数据，这里设置为固定的20字节，该值应该>=包头的实际长度
    uint8_t * recvIndex = nullptr; // 指向当前要接收的数据
    uint16_t recvLength = 0; // 要接收数据的长度
    uint8_t * recvBuffer = nullptr; // 接收缓冲区

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
    TCPConnection * m_freeConnectionPool; // 连接池中可用部分
};

#endif // NGX_C_CONNECTIONPOOL_H_