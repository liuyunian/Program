#include <string.h> // memset
#include <errno.h> // errno
#include <unistd.h> // close
#include <pthread.h> // pthread_mutex_init pthread_mutex_destroy
#include <sys/socket.h> // socket setsockopt
#include <arpa/inet.h> // sockaddr_in

#include "ngx_log.h"
#include "ngx_func.h"
#include "ngx_macro.h"
#include "ngx_c_conf.h"
#include "ngx_c_socket.h"

Socket::Socket() : 
    m_portCount(0),
    m_epfd(-1),
    m_connectionPool(nullptr)
{
    pthread_mutex_init(&m_msgQueMutex, NULL); // 初始化消息队列互斥量
}

Socket::~Socket(){
    // 释放监听套接字
    for(auto & sock : m_listenSokcetList){
        delete sock;
    }
    m_listenSokcetList.clear();

    // 释放消息队列
    ngx_msgQue_clear();

    // 释放消息队列互斥量
    pthread_mutex_destroy(&m_msgQueMutex);
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
            ngx_log(NGX_LOG_FATAL, errno, "Socket::ngx_open_listening_sockets()中执行socket()失败, i = %d", i);
            return false;
        }

        int reuseaddr = 1;
        ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&reuseaddr, sizeof(reuseaddr));
        if(ret < 0){
            ngx_log(NGX_LOG_ERR, errno, "Socket::ngx_open_listening_sockets()中执行setsockopt(SO_REUSEADDR)失败, i = %d", i);
            close(sockfd);                                               
            return false;
        }

        ret = ngx_set_nonblocking(sockfd);
        if(ret < 0){
            ngx_log(NGX_LOG_ERR, errno, "Socket::ngx_open_listening_sockets()中执行ngx_set_nonblocking()失败, i = %d", i);
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
                ngx_log(NGX_LOG_FATAL, 0, "配置文件中没有提供Port%d", i);
                close(sockfd);                                               
                return false;
            }
        }

        serv_addr.sin_port = htons(in_port_t(port));

        ret = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if(ret < 0){
            ngx_log(NGX_LOG_FATAL, errno, "Socket::ngx_open_listening_sockets()中执行bind()失败, i = %d", i);
            close(sockfd);                                               
            return false;
        }

        ret = listen(sockfd, NGX_LISTEN_BACKLOG);
        if(ret < 0){
            ngx_log(NGX_LOG_FATAL, errno, "Socket::ngx_open_listening_sockets()中执行listen()失败, i = %d", i);
            close(sockfd);                                               
            return false;
        }

        ngx_log(NGX_LOG_INFO, 0, "监听%d端口成功", port);
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
        ngx_log(NGX_LOG_INFO, 0, "监听端口%d已关闭", sock->port);
    }
}