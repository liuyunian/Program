/**
 * Socket类是通信功能的核心类(该类最终是否命名为Socket还在犹豫)
 * Socket类作为父类存在，提供通信基本功能
 * 子类继承该类之后，在子类中实现具体的业务逻辑
*/

#include <string.h> // memset
#include <errno.h> // errno
#include <unistd.h> // close
#include <pthread.h> // pthread_mutex_init pthread_mutex_destroy
#include <sys/socket.h> // socket setsockopt
#include <arpa/inet.h> // sockaddr_in

#include "ngx_c_socket.h"

#include "ngx_c_threadPool.h"
#include "app/ngx_log.h"
#include "app/ngx_c_conf.h"
#include "_include/ngx_func.h"
#include "_include/ngx_macro.h"
#include "misc/ngx_c_memoryPool.h"

Socket::Socket() : 
    m_portCount(0),
    m_epfd(-1),
    m_connectionPool(nullptr){}

Socket::~Socket(){
    // 释放监听套接字
    for(auto & sock : m_listenSokcetList){
        delete sock;
    }
    m_listenSokcetList.clear();
}

/********************************************************************
 * 监听套接字有关的函数
*********************************************************************/
int Socket::ngx_sockets_init(){
    ConfFileProcessor * confProcess = ConfFileProcessor::getInstance();
    m_portCount = confProcess->ngx_conf_getContent_int("PortCount", NGX_PROT_COUNT);

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
            ngx_log(NGX_LOG_FATAL, errno, "Socket::ngx_open_listening_sockets()中执行socket()失败, i = %d", i);
            return -1;
        }

        int reuseaddr = 1;
        ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&reuseaddr, sizeof(reuseaddr)); // 设置地址可以复用
        if(ret < 0){
            ngx_log(NGX_LOG_ERR, errno, "Socket::ngx_open_listening_sockets()中执行setsockopt(SO_REUSEADDR)失败, i = %d", i);
            close(sockfd);                                               
            return -1;
        }

        ret = ngx_set_nonblocking(sockfd); // 将socket设置为非阻塞
        if(ret < 0){
            ngx_log(NGX_LOG_ERR, errno, "Socket::ngx_open_listening_sockets()中执行ngx_set_nonblocking()失败, i = %d", i);
            close(sockfd);                                               
            return -1;
        }

        memset(tmp_str, 0, 100);
        sprintf(tmp_str, "Port%d", i);
        port = confProcess->ngx_conf_getContent_int(tmp_str);
        if(port < 0){
            if(m_portCount == 1){
                port = NGX_LISTEN_PORT;
            }
            else{
                ngx_log(NGX_LOG_FATAL, 0, "配置文件中没有提供Port%d", i);
                close(sockfd);                                               
                return -1;
            }
        }

        serv_addr.sin_port = htons(in_port_t(port));

        ret = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if(ret < 0){
            ngx_log(NGX_LOG_FATAL, errno, "Socket::ngx_open_listening_sockets()中执行bind()失败, i = %d", i);
            close(sockfd);                                               
            return -1;
        }

        ret = listen(sockfd, NGX_LISTEN_BACKLOG);
        if(ret < 0){
            ngx_log(NGX_LOG_FATAL, errno, "Socket::ngx_open_listening_sockets()中执行listen()失败, i = %d", i);
            close(sockfd);                                               
            return -1;
        }

        ngx_log(NGX_LOG_INFO, 0, "监听%d端口成功", port);
        ListenSocket * sock = new ListenSocket;
        sock->sockfd = sockfd;
        sock->port = port;
        sock->connection = nullptr;
        m_listenSokcetList.push_back(sock);
    }

    return 0;
}

void Socket::ngx_sockets_close(){
    for(auto & sock : m_listenSokcetList){
        close(sock->sockfd);
        ngx_log(NGX_LOG_INFO, 0, "监听端口%d已关闭", sock->port);
    }
}

/********************************************************************
 * epoll有关的函数
*********************************************************************/
int Socket::ngx_epoll_init(){
    ConfFileProcessor * confProcess = ConfFileProcessor::getInstance();
    int connectionSize = confProcess->ngx_conf_getContent_int("WorkerConnections", NGX_WORKER_CONNECTIONS);

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

        if(ngx_epoll_operateEvent(sock->sockfd, EPOLL_CTL_ADD, EPOLLIN|EPOLLRDHUP, c) < 0){
            return -1;
        }
    }

    return 0;
}

int Socket::ngx_epoll_operateEvent(int sockfd, uint32_t operation, uint32_t eventType, TCPConnection * c){
    struct epoll_event et;
    memset(&et, 0, sizeof(struct epoll_event));

    if(operation == EPOLL_CTL_ADD){ // 增加事件
        et.data.ptr = (void *)((uintptr_t)c | c->validFlag); // 将TCPConnection对象和表征该连接是否有效的标志位放到epoll_event.data中，后续来事件时，取用连接对象和标志位
        et.events = eventType;
    }
    else if(operation == EPOLL_CTL_MOD){ // 修改事件
        // ... 待扩展
    }
    else if(operation == EPOLL_CTL_DEL){ // 删除事件
        // ... 待扩展
    }
    else{ // 无效的操作
        ngx_log(NGX_LOG_ERR, 0, "Socket::ngx_epoll_operateEvent()参数operation没有对应的操作");
        return -1;
    }

    if(epoll_ctl(m_epfd, operation, sockfd, &et) < 0){
        ngx_log(NGX_LOG_ERR, errno, "Socket::ngx_epoll_operateEvent()中执行epoll_ctl()失败");
        return -1;
    }

    return 0;
}

int Socket::ngx_epoll_getEvent(int timer){
    int events = epoll_wait(m_epfd, m_events, NGX_MAX_EVENTS, timer);
    if(events < 0){ // 表示出错
        if(errno == EINTR){ // 信号中断错误
            /**
             * 在等待事件发生过程中，被信号中断，这是经常发生的，再次等待即可
             * 处理方式：函数直接返回，因为返回之后是worker进程的工作循环，会再次调用该函数再次等待事件发生
            */
            return 0;
        }
        else{
            /**
             * 其他错误
            */
            ngx_log(NGX_LOG_FATAL, errno, "Socket::ngx_epoll_getEvent()中执行epoll_wait()失败");
            return -1;
        }
    }
    else if(events == 0){ // 0个事件发生
        if(timer == -1){ // timer等于-1时会一直阻塞，直到事件发生，此时events不可能为0
            ngx_log(NGX_LOG_ERR, 0, "Socket::ngx_epoll_getEvent()中epoll_wait()没超时却没返回任何事件");
            return -1;
        }

        return 0;
    }

    // 收到了events个IO事件，下面进行处理
    TCPConnection * c;
    uintptr_t validFlag;
    uint32_t eventType;
    for(int i = 0; i < events; ++ i){
        validFlag = (uintptr_t) (m_events[i].data.ptr) & 1;
        c = (TCPConnection *) ((uintptr_t)(m_events[i].data.ptr) & (uintptr_t) ~1);

        /**
         * 事件过期原因分析
         * 由于客户端随时可能退出，而退出还分为正常退出和非正常退出：
         *      正常退出：客户端调用close关闭套接字，此时协议栈会进行四次回收
         *      非正常退出：客户端进程直接关闭
         * 
         * 三次握手之后，在服务器端accept之前客户端正常退出或者非正常退出 -- 服务器端如何知道？？ 调用accept()函数出错，errno为ECONNABORTED
         * 
         * 传输数据过程中，客户端正常退出 -- 服务器端如何知道？？ 调用accept()返回值为0
         * 传输数据过程中，客户端非正常退出 -- 服务器端如何知道？？ 调用accept()出错，errno为ECONNABORTED
         * 
         * 客户端与服务器端的连接断开之后的，客户端的事件就属于过期事件
         * 比如：m_events[]中有两个事件，可能吗？
         * 事件1：客户端A关闭了连接
         * 事件2：收到了客户端A所发来的数据
         * 这种情况下，事件2就是一个过期事件，对该事件的处理是没有意义的，所以要过滤掉该类过期事件，也就是通过如下逻辑
        */
        if(c->sockfd == -1){
            /**
             * 关闭连接时，在ngx_event_close()函数中会将c->sockfd置为-1
             * 这样在这检测该事件所属的TCP连接对应的sockfd是否为-1，如果为-1，那么表明TCP连接直接已经断开了，该事件是一个过期事件
            */
            ngx_log(NGX_LOG_DEBUG, 0, "Socket::ngx_epoll_getEvent()中遇到了过期事件");
            continue;
        }

        if(c->validFlag != validFlag){ // 表示该事件是过期事件
            /**
             * 这里是对过期事件进一步过滤
             * 对于下面这种情况，上面没法过滤
             * m_events[]中有三个事件
             * 事件1：客户端A关闭了连接（连接对应的连接池中的TCPConnection对象被回收）
             * 事件2：客户端B连入服务器，采用的sockfd和TCPConnection对象都和客户端A相同（只有sockfd一样才会出现在同一个事件队列中）
             * 事件3：收到了客户端A所发来的数据
             * 
             * 这种情况很少见，且不说sockfd相同的可能性，采用相同的TCP对象的可能性也很小
             * 因为每次回收的连接对象都是放在空闲连接队列的末尾的，如果不是连接池中的可用连接过少，短时间内复用两个连接的可能性很小
             * 所以这种情况在文件描述符资源和连接池资源快耗尽是可能会出现
            */

            ngx_log(NGX_LOG_DEBUG, 0, "Socket::ngx_epoll_getEvent()中遇到了过期事件");
            continue;
        }

        // 开始处理正常的事件
        eventType = m_events[i].events; // 事件类型
        if(eventType & (EPOLLERR | EPOLLHUP)){
            /**
             * 这种情况是什么意思
             * 为什么要这样处理呢？
            */
            eventType |= EPOLLIN | EPOLLOUT; // 增加读写标记
        }
        else if(eventType & EPOLLIN){
            /**
             * 读事件，发生读事件有两种情况
             * 监听套接字：新客户端发起连接 -- 执行ngx_event_accept(c);
             * 连接套接字：客户端发送数据 -- 执行ngx_event_recv(c);
            */
            
            (this->*(c->r_handler))(c);
        }
        else if(eventType & EPOLLOUT){ // 写事件
            /**
             * 写事假
            */

            // ... 后续增加
        }
        else{
            ngx_log(NGX_LOG_ERR, 0, "Socket::ngx_epoll_getEvent()中遇到了未知事件类型");
            return -1;
        }
    }

    return 0;
}

/********************************************************************
 * 事件有关的函数
*********************************************************************/
void Socket::ngx_event_accept(TCPConnection * c){
    struct sockaddr cliAddr;
    socklen_t addrlen;
    int errnoCp;                    // 拷贝errno
    int sockfd;                     // TCP连接对应的socket描述符
    bool accept4Flag = true;                // 标记是否使用accept4，accept4函数是linux特有的扩展函数
    TCPConnection * newConnection;  // 与新连接绑定

    addrlen = sizeof(struct sockaddr);
    while(1){
        if(accept4Flag){
            sockfd = accept4(c->sockfd, &cliAddr, &addrlen, SOCK_NONBLOCK);
        }
        else{
            sockfd = accept(c->sockfd, &cliAddr, &addrlen);
        }

        if(sockfd == -1){ // 发生了错误
            errnoCp = errno;

            /**
             * 根据errno值进行异常处理
             */
            if(errnoCp == ENOSYS && accept4Flag){ // 不支持accept4
                accept4Flag = false;
                continue;
            }
            // else if(errnoCp == EAGAIN | errnoCp == EWOULDBLOCK){
                /**
                 * 套接字标记为非阻塞并且没有连接等待接受
                 * 这种情况应该不会发生，因为这个事件被触发表明一定会有连接等待接受
                */
            // }
            else if(errnoCp == ECONNABORTED){
                /**
                 * 三次握手完成之后，客户端关闭套接字，向服务器端发送了一个RST报文段，此时accpet产生错误，错误码为ECONNABORTED
                 * 处理方式：由于连接已经无效，所以此次触发事件也就无效了，return返回
                */

                return;
            }
            else if(errnoCp == EMFILE || errnoCp == ENFILE){
                /**
                 * EMFILE -- 进程文件描述符用尽
                 * ENFILE -- 达到操作系统允许打开文件个数的全局上线
                 * 处理方式：修改操作系统的配置信息
                */

                //如何处理...

                ngx_log(NGX_LOG_ERR, errnoCp, "ngx_event_accpet()函数中执行accept()函数失败");
                return;
            }
            else{
                /**
                 * 其他错误
                 * EBADF: 描述符无效
                 * ENOBUFS/ENOMEM: 没有足够的内存
                 * ...
                */
                ngx_log(NGX_LOG_FATAL, errnoCp, "ngx_event_accpet()函数中执行accept()函数失败");
                return;
            }
        }

        // 处理accept成功的情况
        if(!accept4Flag){
            if(ngx_set_nonblocking(sockfd) < 0){
                ngx_log(NGX_LOG_ERR, errno, "ngx_event_accpet()中执行ngx_set_nonblocking()失败");
                close(sockfd);                                             
                return;
            }
        }

        newConnection = m_connectionPool->ngx_get_connection(sockfd);
        if(newConnection == nullptr){ // 直接错误返回，因为ngx_get_connection()函数中已经写过日志了
            close(sockfd);
            return;
        }

        // 需判断是否超过最大允许的连接数...

        // memcpy(&newConnection->cliAddr, &cliAddr, addrlen);
        // newConnection->w_ready = 1; // 已准备好写数据
        newConnection->r_handler = &Socket::ngx_event_recv;
        newConnection->curRecvPktState = RECV_PKT_HEADER; // 收包状态处于准备接收包头状态
        newConnection->recvIndex = newConnection->pktHeader;
        newConnection->recvLength = sizeof(PktHeader);
        
        if(ngx_epoll_operateEvent(sockfd, EPOLL_CTL_ADD, EPOLLIN|EPOLLRDHUP, newConnection) < 0){ // LT模式
            /**
             * 向epoll对象添加事件出错，出错的原因是epoll_ctl()函数出错
             * 虽然这个地方不易出错，但是对于这样的错误处理方式是否不妥
             * 该连接已经被accept,所以之后是不会再触发该事件了，那么这里一旦返回，那么此次连接将不可能进行数据的收发
             * 猜测客户端应该发现已经连接上了服务器，但是没法进行数据收发，那么客户端可能要采取一定的措施发现这个问题
             * 
             * 应该如何处理呢？
             * 多次尝试epoll_ctl？？？
            */

            ngx_event_close(newConnection);
            return;
        }

        break;
    }
}

void Socket::ngx_event_close(TCPConnection * c){
    close(c->sockfd);
    c->sockfd = -1; // 使用sockfd判断连接是否已过期
    m_connectionPool->ngx_free_connection(c);
}

void Socket::ngx_event_recv(TCPConnection * c){
    ngx_log(NGX_LOG_DEBUG, 0, "执行了ngx_event_recv()函数");

    ssize_t len = recv(c->sockfd, c->recvIndex, c->recvLength, 0); // recv()系统调用，这里flags设置为0，作用同read()

    if(len < 0){
        int errnoCp = errno;
        // if(errnoCp == EAGAIN || errnoCp == EWOULDBLOCK){
            /**
             * 表示没有从接收缓冲区中拿到数据
             * 因为epoll工作在LT模式下，触发该事件表明缓冲区中有数据，所以这个错误不会发生
             * 只有在ET模式下才会出现该错误，ET模式下从缓存区中拿数据是要在循环中不断的调用recv()，当recv()函数返回-1，并且errno = EAGAIN || EWOULDBLOCK，表示缓冲区中没有数据了，此次接收数据结束
            */
        // }

        if(errnoCp == EINTR){
            /**
             * 表示函数执行过程中被信号打断
             * 一般函数阻塞等待的过程中才发生该错误，这里sockfd标记为非阻塞，所以recv()函数也是非阻塞的，可能发生该错误吗？
             * 处理方式：直接return即可，因为LT模式下，该事件还会被再次触发
            */
            return;
        }

        else if(errnoCp == ECONNRESET){ // 客户端没有正常的关闭连接，此时服务器端也应关闭连接
            /**
             * 发送数据阶段，客户端非正常关闭连接，此时服务器也应关闭连接
             * 处理方式：关闭连接并返回 
            */

            ngx_event_close(c);
            return;
        }
        else{
            /**
             * ... 其他错误等待后续完善
            */

            ngx_log(NGX_LOG_ERR, errno, "ngx_event_recv()函数中调用recv()函数接收数据错误");
            return;
        }
    }

    if(len == 0){ // 表示客户端关闭连接，服务器端也应关闭连接
        ngx_event_close(c);
        return;
    }

    // 处理正确接收的情况，采用状态机
    if(c->curRecvPktState == RECV_PKT_HEADER){
        if(len == c->recvLength){ // 接收到了完整的包头
            ngx_pktHeader_parsing(c);
        }
        else{
            c->recvIndex += len;
            c->recvLength -= len;
        }
    }
    else if(c->curRecvPktState == RECV_PKT_BODY){
        if(len == c->recvLength){ // 接收到了完整的包体
            ngx_packet_handle(c);
        }
        else{
            c->recvIndex += len;
            c->recvLength -= len;
        }
    }
    else{
        ngx_log(NGX_LOG_ERR, 0, "ngx_event_recv()函数中TCP连接对象c处于无效的收包状态，%d", c->curRecvPktState);
        return;
    }
}

void Socket::ngx_pktHeader_parsing(TCPConnection * c){
    ngx_log(NGX_LOG_DEBUG, 0, "执行了ngx_pktHeader_parsing()函数");

    uint16_t pktHd_len = static_cast<uint16_t>(sizeof(PktHeader));

    PktHeader * pktHd = (PktHeader *)(c->pktHeader);
    uint16_t pktLen = ntohs(pktHd->len); // 拿到完整的包长信息
    if(pktLen < pktHd_len){ // 记录的包长小于包头长度，此时数据包无效
        c->curRecvPktState = RECV_PKT_HEADER;
        c->recvIndex = c->pktHeader;
        c->recvLength = pktHd_len;

        // ...这里处理有问题，没有将缓冲区中包体丢弃
        return;
    }

    if(pktLen > PKT_MAX_LEN){ // 对于大于PKT_MAX_LEN(10240)字节的包，认为也是无效包
        c->curRecvPktState = RECV_PKT_HEADER;
        c->recvIndex = c->pktHeader;
        c->recvLength = pktHd_len;

        // ...这里处理有问题，没有将缓冲区中包体丢弃
        return;
    }

    ngx_log(NGX_LOG_DEBUG, 0, "收到正确的数据包");

    // 处理正确的情况
    // [1] 分配内存，用于存放消息头+包头+包体
    uint8_t * buf = (uint8_t *)MemoryPool::getInstance()->ngx_alloc_memory(sizeof(struct MsgHeader) + pktLen);
    c->recvBuffer = buf;

    // [2] 填写消息头
    MsgHeader * msgHd = (MsgHeader *)buf;
    msgHd->c = c;
    msgHd->curSeq = c->curSeq;

    // [3] 填充包头
    buf += sizeof(MsgHeader);
    memcpy(buf, c->pktHeader, pktHd_len);

    if(pktLen == pktHd_len){ // 只有包头没有包体的情况，这种情况是允许的
        ngx_packet_handle(c);
    }
    else{
        c->curRecvPktState = RECV_PKT_BODY;
        c->recvIndex = buf + pktHd_len; // buf当前指向包头，后移sizeof(PktHeader)指向包体
        c->recvLength = pktLen - pktHd_len;
    }
}

void Socket::ngx_packet_handle(TCPConnection * c){
    ngx_log(NGX_LOG_DEBUG, 0, "执行了ngx_packet_handle()函数");

    // [1] 将接收到的完整的数据push到消息队列
    ThreadPool * tp = ThreadPool::getInstance();
    tp->ngx_msgQue_push(c->recvBuffer);

    // [2] 准备接收后续的数据
    c->recvBuffer = nullptr; // 对应的内存由消息队列接管
    c->curRecvPktState = RECV_PKT_HEADER;
    c->recvIndex = c->pktHeader;
    c->recvLength = sizeof(PktHeader);
}