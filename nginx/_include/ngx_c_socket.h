#ifndef NGX_C_SOCKET_H_
#define NGX_C_SOCKET_H_

#include <vector>

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

class Socket{
public:
    Socket();
    ~Socket();

    bool ngx_sockets_init(); // 打开监听套接字

	void ngx_socket_close(); // 关闭监听套接字

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
    void ngx_event_accpet(TCPConnection * c);

    void ngx_event_close(TCPConnection * c);

    void ngx_event_recv(TCPConnection * c);

private:
    int m_portCount; // 监听端口的数目
    int m_epfd; // 表示epoll对象

    struct epoll_event m_events[NGX_MAX_EVENTS]; // 记录epoll对象中发生的事件

    ConnectionPool * m_connectionPool; // 连接池对象
    std::vector<ListenSocket *> m_listenSokcetList; // 监听socket列表
};

#endif // NGX_C_SOCKET_H_