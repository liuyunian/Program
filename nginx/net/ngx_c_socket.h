/**
 * Socket类是通信功能的核心类(该类最终是否命名为Socket还在犹豫)
 * Socket类作为父类存在，提供通信基本功能
 * 子类继承该类之后，在子类中实现具体的业务逻辑
*/

#ifndef NGX_C_SOCKET_H_
#define NGX_C_SOCKET_H_

#include <vector>
#include <list>

#include <sys/epoll.h> // epoll_event

#include "ngx_c_connectionPool.h"
#include "_include/ngx_macro.h"

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
    uint16_t type; // 类型
    int crc32; // 用于CRC32校验
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
    u_int64_t curSeq;  // 连接对象的序号，用于判断连接是否作废
    // ...扩展
};

class Socket{
public:
    Socket();
    virtual ~Socket(); // 父类中的析构函数一定要是虚函数

    int ngx_sockets_init(); // 打开监听套接字

	void ngx_sockets_close(); // 关闭监听套接字

    int ngx_epoll_init();

    /**
     * @brief 操作epoll对象，对事件进行增、删、改
     * @param sockfd socket描述符
     * @param operation要进行的动作，epoll_ctl中的参数op -- EPOLL_CTL_ADD、EPOLL_CTL_MOD、EPOLL_CTL_DEL
     * @param eventType事件类型 -- EPOLLIN读事件、EPOLLRDHUP挂起事件
     * @param c表示一个连接
     * @return 执行成功返回0， 否则返回-1
     */
    int ngx_epoll_operateEvent(int sockfd, uint32_t operation, uint32_t eventType, TCPConnection * c);

    /**
     * @brief 从epoll对象中获取发生的事件
     * @param timer作为epoll_wait的参数
     * @return 成功执行返回0， 否则返回-1
     */
    int ngx_epoll_getEvent(int timer);

    /**
     * @brief 处理从消息队列中拿到的消息
     * 父类中声明为纯虚函数，子类中必须具体实现
    */
   virtual void ngx_msg_handle(uint8_t * msg) = 0; 

private:
    // epoll事件回调
    void ngx_event_accept(TCPConnection * c);

    void ngx_event_close(TCPConnection * c);

    void ngx_event_recv(TCPConnection * c);

    /**
     * @brief 解析收到的包头信息
    */
    void ngx_pktHeader_parsing(TCPConnection * c);

    /**
     * @brief 处理接收到的完成的数据包
    */
    void ngx_packet_handle(TCPConnection * c);

private:
    int m_portCount; // 监听端口的数目
    int m_epfd; // 表示epoll对象

    struct epoll_event m_events[NGX_MAX_EVENTS]; // 记录epoll对象中发生的事件

    ConnectionPool * m_connectionPool; // 连接池对象
    std::vector<ListenSocket *> m_listenSokcetList; // 监听socket列表
};

#endif // NGX_C_SOCKET_H_