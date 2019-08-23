/**
 * Socket类是通信功能的核心类(该类最终是否命名为Socket还在犹豫)
 * Socket类作为父类存在，提供通信基本功能
 * 子类继承该类之后，在子类中实现具体的业务逻辑
*/

#ifndef NGX_C_SOCKET_H_
#define NGX_C_SOCKET_H_

#include <vector>
#include <list>
#include <atomic>

#include <pthread.h> // 线程
#include <semaphore.h> // 信号量 
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

    /**
     * @brief 从epoll对象中获取发生的事件
     * @param timer作为epoll_wait的参数
     * @return 成功执行返回0， 否则返回-1
     */
    int ngx_epoll_getEvent(int timer);

protected:
    /**
     * @brief Socket对象在master进程中的初始化
    */
    virtual int ngx_socket_master_init();

    /**
     * @brief Socket对象在worker进程中的初始化
    */
    virtual int ngx_socket_worker_init();

    /**
     * @brief Socket对象在master进程中释放
    */
    virtual void ngx_socket_master_destroy();

    /**
     * @brief Socket对象在master进程中释放
    */
    virtual void ngx_socket_worker_destroy();

    /**
     * @brief 处理从消息队列中拿到的消息
     * 父类中声明为纯虚函数，子类中必须具体实现
    */
   virtual void ngx_recvMsg_handle(uint8_t * msg) = 0;

    /**
    * @brief 将要发送的数据添加到发送队列中
    */
    void ngx_sendMsgQue_push();

private:
    /**
     * @brief 初始化监听套接字
    */
    int ngx_listenSockets_init();

    /**
     * 关闭监听套接字
    */
    void ngx_listenSockets_close();

    /**
     * @brief 初始化epoll对象
    */
    int ngx_epoll_init();

    /**
     * @brief 关闭epoll对象
    */
    void ngx_epoll_close();

    /**
     * @brief 操作epoll对象，对事件进行增、删、改
     * @param sockfd socket描述符
     * @param operation要进行的动作，epoll_ctl中的参数op -- EPOLL_CTL_ADD、EPOLL_CTL_MOD、EPOLL_CTL_DEL
     * @param option 在operation为EPOLL_CTL_MOD时，该参数进一步说明是增加监听事件、减少监听事件或者覆盖之前的监听事件
     * @param eventType事件类型 -- EPOLLIN读事件、EPOLLRDHUP挂起事件
     * @param c表示一个连接
     * @return 执行成功返回0， 否则返回-1
     */
    int ngx_epoll_operateEvent(int sockfd, uint32_t operation, int option, uint32_t eventType, TCPConnection * c);

    /**
     * @brief 监听套接字接入连接事件
    */
    void ngx_event_accept(TCPConnection * c);

    /**
     * @brief 连接关闭事件
    */
    void ngx_event_close(TCPConnection * c);

    /**
     * @brief 收到数据事件
    */
    void ngx_event_recv(TCPConnection * c);

    /**
     * @brief 解析收到的包头信息
    */
    void ngx_pktHeader_parsing(TCPConnection * c);

    /**
     * @brief 处理接收到的完成的数据包
    */
    void ngx_packet_handle(TCPConnection * c);

    /**
     * @brief 可以发送数据事件
    */
    void ngx_event_send(TCPConnection * c);

    /**
     * @brief 发送消息线程入口函数
     * 静态成员函数
    */
    static void * ngx_sendMsg_thread_entryFunc(void * arg);

    /**
     * @brief 对send()函数做进一步封装
    */
    ssize_t ngx_send(int sockfd, uint8_t * buf, size_t nbytes);

private:
    // 静态成员常量
    static const int PKT_HEADER_SZ;
    static const int MSG_HEADER_SZ;

    // int m_portCount; // 监听端口的数目，感觉没有必要作为成员变量存在
    std::vector<ListenSocket *> m_listenSokcetList; // 监听socket列表

    int m_epfd; // 表示epoll对象
    struct epoll_event m_events[NGX_MAX_EVENTS]; // 记录epoll对象中发生的事件

    ConnectionPool * m_connectionPool; // 连接池对象

    /**
     * 发送消息队列
     * 发送数据的逻辑处理决定了这里不能选用queue容器
     * 因为这里存放的是多条连接上要发送的消息，多条连接的发送缓冲区的状态是不一样的
     * 有的连接空闲那么就需要发送消息并从队列中移除消息，而有的发送缓冲区已满，那么消息就需要暂存在队列中
     * 因此对发送消息的处理不符合“先入先出”的原则，不能选用queue
    */
    std::list<uint8_t *> m_sendMsgQue;
    pthread_mutex_t m_sendMsgQueMutex;  // 互斥量
    sem_t m_sendThreadSem;              // 信号量

    pthread_t m_sendTid;
    std::atomic_bool m_sendThreadStop;
};

#endif // NGX_C_SOCKET_H_