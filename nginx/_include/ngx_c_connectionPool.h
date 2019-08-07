#ifndef NGX_C_CONNECTIONPOOL_H_
#define NGX_C_CONNECTIONPOOL_H_

#include <arpa/inet.h> // sockaddr_in

struct TCPConnection;

using ngx_event_handle = void (*)(TCPConnection *);

/**
 * @brief 表示一个TCP连接
 */
struct TCPConnection{
    int sockfd; // socket描述符
    unsigned instance:1; // 失效标志位
    u_int64_t curSeq; // ??
    struct sockaddr cliAddr; // 客户端的地址信息

    uint8_t r_ready; // 读准备好标记
    uint8_t w_ready; // 写准备好标记

    ngx_event_handle r_handler; // 读事件的处理方法
    ngx_event_handle w_handler; // 写事件的处理方法

    TCPConnection * next; // 指向下一个TCP连接
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