#include <string.h> // memset
#include <errno.h> // errno
#include <unistd.h> // close
#include <sys/socket.h> // socket setsockopt
#include <sys/ioctl.h> //ioctl
#include <sys/epoll.h> // epoll_create
#include <arpa/inet.h> // sockaddr_in

#include "ngx_func.h"
#include "ngx_macro.h"
#include "ngx_c_conf.h"
#include "ngx_c_socket.h"
#include "ngx_c_connectionPool.h"

Socket::Socket() : 
    m_portCount(0),
    m_epfd(-1),
    m_connectionPool(nullptr)
    {}

Socket::~Socket(){
    // 释放监听套接字
    for(auto & sock : m_listenSokcetList){
        delete sock;
    }
    m_listenSokcetList.clear();
}

bool Socket::ngx_sockets_init(){
    ConfFileProcessor * confProcess = ConfFileProcessor::getInstance();
    m_portCount = confProcess->getItemContent_int("PortCount", NGX_PROT_COUNT);

    int sockfd;
    struct sockaddr_in serv_addr;
    int port;
    char tmp_str[100];
    int ret; // 记录返回值

    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    for(int i = 1; i <= m_portCount; ++ i){
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0){
            log(NGX_LOG_CRIT, errno, "Socket::ngx_open_listening_sockets()中执行socket()失败, i = %d", i);
            return false;
        }

        //@setsockopt()：设置一些套接字参数选项
        //参数2：是表示级别，和参数3配套使用，也就是说，参数3如果确定了，参数2就确定了;
        //参数3：允许重用本地地址
        //设置 SO_REUSEADDR，目的第五章第三节讲解的非常清楚：主要是解决TIME_WAIT这个状态导致bind()失败的问题
        int reuseaddr = 1;  //1:打开对应的设置项
        ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&reuseaddr, sizeof(reuseaddr));
        if(ret < 0){
            log(NGX_LOG_CRIT, errno, "Socket::ngx_open_listening_sockets()中执行setsockopt(SO_REUSEADDR)失败, i = %d", i);
            close(sockfd);                                               
            return false;
        }

        ret = ngx_set_nonblocking(sockfd);
        if(ret < 0){
            log(NGX_LOG_ERR, errno, "Socket::ngx_open_listening_sockets()中执行ngx_set_nonblocking()失败, i = %d", i);
            close(sockfd);                                               
            return false;
        }

        memset(tmp_str, 0, 100);
        sprintf(tmp_str, "Port%d", i);
        port = confProcess->getItemContent_int(tmp_str);
        if(port < 0){
            if(m_portCount == 1){
                port = NGX_LISTEN_PORT;
            }
            else{
                log(NGX_LOG_ERR, 0, "配置文件中没有提供Port%d", i);
                close(sockfd);                                               
                return false;
            }
        }

        serv_addr.sin_port = htons(in_port_t(port));

        ret = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if(ret < 0){
            log(NGX_LOG_CRIT, errno, "Socket::ngx_open_listening_sockets()中执行bind()失败, i = %d", i);
            close(sockfd);                                               
            return false;
        }

        ret = listen(sockfd, NGX_LISTEN_BACKLOG);
        if(ret < 0){
            log(NGX_LOG_CRIT, errno, "Socket::ngx_open_listening_sockets()中执行listen()失败, i = %d", i);
            close(sockfd);                                               
            return false;
        }

        log(NGX_LOG_INFO, 0, "监听%d端口成功", port);
        ListenSocket * sock = new ListenSocket;
        sock->sockfd = sockfd;
        sock->port = port;
        sock->connection = nullptr;
        m_listenSokcetList.push_back(sock);
    }

    return true;
}

void Socket::ngx_sockets_close(){
    for(auto & sock : m_listenSokcetList){
        close(sock->sockfd);
        log(NGX_LOG_INFO, 0, "监听端口%d已关闭", sock->port);
    }
}

int Socket::ngx_set_nonblocking(int sockfd){
    int nb = 1; // 0：清除，1：设置  
    if(ioctl(sockfd, FIONBIO, &nb) < 0){ //FIONBIO：设置/清除非阻塞I/O标记：0：清除，1：设置
        return -1;
    }

    return 0;
}

int Socket::ngx_epoll_init(){
    ConfFileProcessor * confProcess = ConfFileProcessor::getInstance();
    int connectionSize = confProcess->getItemContent_int("WorkerConnections", NGX_WORKER_CONNECTIONS);

    // [1] 创建epoll对象
    m_epfd = epoll_create(connectionSize);
    if(m_epfd < 0){
        log(NGX_LOG_CRIT, errno, "ngx_epoll_init()中调用epoll_create()函数失败");
        return -1;
    }

    // [2] 创建连接池
    m_connectionPool = new ConnectionPool(connectionSize);

    // [3] 为每个监听套接字绑定一个连接池中的连接
    TCPConnection * c;
    for(auto & sock : m_listenSokcetList){
        c = m_connectionPool->ngx_get_connection(sock->sockfd);
        if(c == nullptr){
            log(NGX_LOG_CRIT, 0, "ngx_epoll_init()中执行ngx_get_connection()失败");
            return -1;
        }
        sock->connection = c;

        //  c->r_handler = &ngx_event_accept;

        if(ngx_epoll_addEvent(sock->sockfd, 1, 0, 0, EPOLL_CTL_ADD, c) < 0){
            return -1;
        }
    }

    return 0;
}

int Socket::ngx_epoll_addEvent(int sockfd, 
                                int r_event, int w_event, 
                                uint32_t otherFlags, 
                                uint32_t eventType, 
                                TCPConnection * c){

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
        log(NGX_LOG_CRIT, errno, "Socket::ngx_epoll_addEvent()中执行epoll_ctl()失败");
        return -1;
    }

    return 0;
}

int Socket::ngx_epoll_processEvent(){

}