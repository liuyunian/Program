/**
 * Socket类中与epoll有关的函数
 */
#include <sys/epoll.h>

#include "ngx_log.h"
#include "ngx_func.h"
#include "ngx_macro.h"
#include "ngx_c_conf.h"
#include "ngx_c_socket.h"
#include "ngx_c_connectionPool.h"


int Socket::ngx_epoll_init(){
    ConfFileProcessor * confProcess = ConfFileProcessor::getInstance();
    int connectionSize = confProcess->getItemContent_int("WorkerConnections", NGX_WORKER_CONNECTIONS);

    // [1] 创建epoll对象
    m_epfd = epoll_create(connectionSize);
    if(m_epfd < 0){
        ngx_log(NGX_LOG_FATAL, errno, "ngx_epoll_init()中调用epoll_create()函数失败");
        return -1;
    }

    // [2] 创建连接池
    m_connectionPool = new ConnectionPool(connectionSize);

    // [3] 为每个监听套接字绑定一个连接池中的连接
    TCPConnection * c;
    for(auto & sock : m_listenSokcetList){
        c = m_connectionPool->ngx_get_connection(sock->sockfd);
        if(c == nullptr){ // 直接错误返回，因为ngx_get_connection()函数中已经写过日志了
            return -1;
        }
        sock->connection = c;

        c->r_handler = &Socket::ngx_event_accept;

        if(ngx_epoll_addEvent(sock->sockfd, 1, 0, 0, EPOLL_CTL_ADD, c) < 0){
            return -1;
        }
    }

    return 0;
}

int Socket::ngx_epoll_addEvent(int sockfd, int r_event, int w_event, uint32_t otherFlags, uint32_t eventType, TCPConnection * c){
    struct epoll_event et;
    memset(&et, 0, sizeof(struct epoll_event));
    if(r_event == 1){
        et.events = EPOLLIN|EPOLLRDHUP;
    }
    else{
        // 其他事件类型待处理
    }

    if(otherFlags != 0){
        et.events |= otherFlags;
    }

    et.data.ptr = (void *)( (uintptr_t)c | c->instance);   //把对象弄进去，后续来事件时，用epoll_wait()后，这个对象能取出来用 
                                                             //但同时把一个 标志位【不是0就是1】弄进去

    if(epoll_ctl(m_epfd, eventType, sockfd, &et) < 0){
        ngx_log(NGX_LOG_FATAL, errno, "Socket::ngx_epoll_addEvent()中执行epoll_ctl()失败");
        return -1;
    }

    return 0;
}

int Socket::ngx_epoll_getEvent(int timer){
    int events = epoll_wait(m_epfd, m_events, NGX_MAX_EVENTS, timer);
    if(events < 0){ // 表示出错
        if(errno == EINTR){ // 信号中断错误
            ngx_log(NGX_LOG_INFO, errno, "Socket::ngx_epoll_processEvent()中因信号中断导致执行epoll_wait()失败");
            return 0; // 这种错误算作正常情况
        }
        else{
            ngx_log(NGX_LOG_FATAL, errno, "Socket::ngx_epoll_processEvent()中执行epoll_wait()失败");
            return -1;
        }
    }
    else if(events == 0){
        if(timer == -1){ // timer等于-1时会一直阻塞，直到事件发生，此时events不可能为0
            ngx_log(NGX_LOG_ERR, 0, "Socket::ngx_epoll_processEvent()中epoll_wait()没超时却没返回任何事件");
            return -1;
        }

        return 0;
    }

    // 收到了events个IO事件，下面进行处理
    TCPConnection * c;
    uintptr_t instance;
    uint32_t eventType;
    for(int i = 0; i < events; ++ i){
        c = (TCPConnection *)(m_events[i].data.ptr);
        instance = (uintptr_t) c & 1;
        c = (TCPConnection *) ((uintptr_t)c & (uintptr_t) ~1);

        if(c->sockfd == -1 || c->instance != instance){ // 表示该事件是过期事件
            ngx_log(NGX_LOG_DEBUG, 0, "Socket::ngx_epoll_processEvent()中遇到了过期事件");
            continue; // 对于过期事件不予处理
        }

        // 开始处理正常的事件
        eventType = m_events[i].events; // 事件类型
        if(eventType & (EPOLLERR | EPOLLHUP)){ // 错误类型
            eventType |= EPOLLIN | EPOLLOUT; // 增加读写标记
        }

        if(eventType & EPOLLIN){ // 读事件，两种情况：新客户端连入事件；已连接的客户端发送数据事件
            (this->*(c->r_handler))(c); // 如果是新客户端连接事件，那么执行ngx_event_accept(c);
                                        // 如果是已连接的客户端发送数据事件，那么执行ngx_event_recv(c)
        }

        if(eventType & EPOLLOUT){ // 写事件
            //后续增加...
        }
    }

    return 0;
}