#ifndef NGX_C_SOCKET_H_
#define NGX_C_SOCKET_H_

#include <vector>
#include <list>

// #include <pthread.h>
#include <sys/epoll.h> // epoll_event

#include "ngx_macro.h"
#include "ngx_c_connectionPool.h"

/**
 * @brief 表示一个监听套接字
 */
struct ListenSocket{
    int sockfd;
    int port;
    TCPConnection * connection;
};

/**
 * @brief 包头
 * 客户端和服务器端协商的数据包格式：包头+包体
 */
#pragma pack(1) // 采用1字节对齐

struct PktHeader{
    uint16_t len; // 记录数据包的长度
    uint16_t msgType; // 记录消息类型
    // ... 待扩展
};

#pragma pack()

/**
 * @brief 消息头
 * 服务器端保存客户端发来的数据的格式是：消息头+包头+包体
 * 消息头的作用是：记录一些额外的信息，后面会用到
 */
struct MsgHeader{
    TCPConnection * c; // 连接对象
    u_int64_t curSeq;  // 连接对象的序号，用于判断连接是否作废？？
    // ...扩展
};

class Socket{
public:
    Socket();
    ~Socket();

    bool ngx_sockets_init(); // 打开监听套接字

	void ngx_sockets_close(); // 关闭监听套接字

    int ngx_epoll_init();

    /**
     * @brief 向epoll对象添加事件
     * @param sockfd socket描述符
     * @param r_event标识是否是读事件，0不是，1是
     * @param w_event标识是否是写事件，0不是，1是
     * @param otherFlag其他标记
     * @param eventType事件类型，epoll_ctl中的参数op
     * @param c表示一个连接
     * @return 执行成功返回0， 否则返回-1
     */
    int ngx_epoll_addEvent(int sockfd, int r_event, int w_event, uint32_t otherFlags, uint32_t eventType, TCPConnection * c);

    /**
     * @brief 从epoll对象中获取发生的事件
     * @param timer作为epoll_wait的参数
     * @return 成功执行返回0， 否则返回-1
     */
    int ngx_epoll_getEvent(int timer);

private:
    // epoll事件回调
    void ngx_event_accept(TCPConnection * c);

    void ngx_event_close(TCPConnection * c);

    void ngx_event_recv(TCPConnection * c);

    // 处理接收数据
    void ngx_pktHeader_handle(TCPConnection * c);

    void ngx_pkt_handle(TCPConnection * c);

    // 消息队列
    void ngx_msgQue_push(uint8_t * msg);

    uint8_t * ngx_msgQue_pop();

    void ngx_msgQue_clear();

private:
    int m_portCount; // 监听端口的数目
    int m_epfd; // 表示epoll对象

    struct epoll_event m_events[NGX_MAX_EVENTS]; // 记录epoll对象中发生的事件

    ConnectionPool * m_connectionPool; // 连接池对象
    std::vector<ListenSocket *> m_listenSokcetList; // 监听socket列表

    std::list<uint8_t *> m_msgQueue; // 消息队列
    pthread_mutex_t m_msgQueMutex; // 消息队列互斥量
};

#endif // NGX_C_SOCKET_H_