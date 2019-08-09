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
        log(NGX_LOG_CRIT, errno, "Socket::ngx_epoll_addEvent()中执行epoll_ctl()失败");
        return -1;
    }

    return 0;
}

int Socket::ngx_epoll_getEvent(int timer){
    int events = epoll_wait(m_epfd, m_events, NGX_MAX_EVENTS, timer);
    if(events < 0){ // 表示出错
        if(errno == EINTR){ // 信号中断错误
            log(NGX_LOG_INFO, errno, "Socket::ngx_epoll_processEvent()中因信号中断导致执行epoll_wait()失败");
            return 0; // 这种错误算作正常情况
        }
        else{
            log(NGX_LOG_ERR, errno, "Socket::ngx_epoll_processEvent()中执行epoll_wait()失败");
            return -1;
        }
    }
    else if(events == 0){
        if(timer == -1){ // timer等于-1时会一直阻塞，直到事件发生，此时events不可能为0
            log(NGX_LOG_ERR, 0, "Socket::ngx_epoll_processEvent()中epoll_wait()没超时却没返回任何事件");
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
            log(NGX_LOG_DEBUG, 0, "Socket::ngx_epoll_processEvent()中遇到了过期事件");
            continue; // 对于过期事件不予处理
        }

        // 开始处理正常的事件
        eventType = m_events[i].events; // 事件类型
        if(eventType & (EPOLLERR | EPOLLHUP)){ // 错误类型
            eventType |= EPOLLIN | EPOLLOUT; // 增加读写标记
        }

        if(eventType & EPOLLIN){ // 如果是读事件，两种情况：新客户端连入事件；已连接的客户端发送数据事件
            (this->*(c->r_handler))(c); // 如果是新客户端连接事件，那么执行ngx_event_accept(c);
                                        // 如果是已连接的客户端发送数据事件，那么执行ngx_event_recv(c)
        }

        if(eventType & EPOLLOUT){
            //后续增加...
        }
    }

    return 0;
}

void Socket::ngx_event_accpet(TCPConnection * c){
    struct sockaddr cliAddr;
    socklen_t addrlen;
    int errnoCp; // 拷贝errno
    int sockfd; // TCP连接对应的socket描述符
    int accept4_flag; // 标记是否使用accept4，accept4函数是linux特有的扩展函数
    TCPConnection * newConnection; // 与新连接绑定

    addrlen = sizeof(struct sockaddr);
    while(1){
        if(accept4_flag){
            sockfd = accpet4(c->sockfd, &cliAddr, &addrlen, SOCK_NONBLOCK);
        }
        else{
            sockfd = accept(c->sockfd, &cliAddr, &addrlen);
        }

        if(sockfd == -1){ // 发生了错误
            errnoCp = errno;
            if(accept4_flag && errnoCp == ENOSYS){ // 不支持accept4
                accept4_flag = 0;
                continue;
            }

            if(errnoCp == EAGAIN){ // accept()没准备好，这个EAGAIN错误和EWOULDBLOCK是一样的
                //如何处理...
            }

            if(errnoCp == ECONNABORTED){ // 对方意外关闭套接字
                //如何处理...
            }

            if(errnoCp == EMFILE || errnoCp == ENFILE){ // EMFILE进程文件描述符用尽，ENFILE??
                //如何处理...
            }

            log(NGX_LOG_ERR, errnoCp, "ngx_event_accpet()函数中执行accept()函数失败");
            return;
        }

        // 处理accept成功的情况
        if(!accept4_flag){
            if(ngx_set_nonblocking(sockfd) < 0){
                log(NGX_LOG_ERR, errno, "ngx_event_accpet()中执行ngx_set_nonblocking()失败");
                close(sockfd);                                             
                return;
            }
        }

        newConnection = m_connectionPool->ngx_get_connection();
        if(newConnection == nullptr){ // 直接错误返回，因为ngx_get_connection()函数中已经写过日志了
            close(sockfd);
            return;
        }
        // 需判断是否超过最大允许的连接数...

        memcpy(&newConnection->cliAddr, &cliAddr, addrlen);
        newConnection->w_ready = 1; // 已准备好写数据
        newConnection->r_handler = &Socket::ngx_event_recv;
        
        if(ngx_epoll_addEvent(sockfd, 1, 0, EPOLLET, EPOLL_CTL_ADD, newConnection) < 0){
            ngx_event_close(newConnection);
            return;
        }

        break;
    }
}

void Socket::ngx_event_close(TCPConnection * c){
    m_connectionPool->ngx_free_connection(c);
    close(c->sockfd);
    c->sockfd = -1; // 表征该连接已过期
}

void Socket::ngx_event_recv(TCPConnection * c){

}