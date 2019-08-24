#include <stdio.h> // perror
#include <errno.h>

#include <sys/socket.h>

#include "c_server.h"

Server::Server(int port) : 
    m_port(port),
    m_listenSockfd(-1),
    m_epfd(-1){}

Server::~Server(){}

int Server::run(){
    int ret;

    m_listenSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(m_listenSockfd < 0){
        perror("创建监听套接字失败");
        return -1;
    }

    // 设置socket描述符为非阻塞
    set_nonblocking(m_listenSockfd);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(m_port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(m_listenSockfd, (struct sockaddr * )&serv_addr, sizeof(struct sockaddr));
    if(ret < 0){
        perror("监听套接字绑定地址失败");
        return -1;
    }

    ret = listen(m_listenSockfd, BACKLOG);
    if(ret < 0){
        perror("调用listen()函数失败");
        return -1;
    }

    m_epfd = epoll_create(MAX_CONN_NUM);
    if(m_epfd < 0){
        perror("创建epoll对象失败");
    }

    epoll_event ev;
    ev.events = EPOLLIN | EPOLLET; // 注册读事件和ET模式
    ev.data.fd = m_listenSockfd; // 用于判断该事件对应的sockfd
    ret = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_listenSockfd, &ev);
    if(ret < 0){
        perror("向epoll对象添加监听套接字事件失败");
    }

    perror("服务器启动");

    int eventNum = 0;
    int sockfd = 0; // 记录发生事件对应的fd
    int connfd = 0; // 记录accept返回的连接对应的sockfd

    Connection * conn;

    for(;;){
        eventNum = epoll_wait(m_epfd, m_events, MAX_CONN_NUM, -1);
        if(eventNum < 0){
            if(errno == EINTR){
                continue;
            }
            else{
                perror("epoll_wait()执行失败");
            }
        }
        
        for(int i = 0; i < eventNum; ++ i){
            sockfd = m_events[i].data.fd;
            if(m_events[i].events & EPOLLIN){ // 读事件
                if(sockfd == m_listenSockfd){ // 监听套接字事件
                    connfd = accept(serv_fd, (struct sockaddr * )NULL, NULL);
                    if(connfd < 0){
                        perror("accept()执行失败");
                        continue;
                    }

                    set_nonblocking(connfd);

                    conn = new Connection(connfd);
                    conn->m_event.events = EPOLLIN | EPOLLET;
                    conn->m_event.data.fd = connfd;
                    ret = epoll_ctl(m_epfd, EPOLL_CTL_ADD, connfd, &m_connectionStore[connfd].m_event);
                    if(ret < 0){
                        perror("向epoll对象添加监听套接字事件失败");
                        continue;
                    }

                    perror("一个连接接入");
                }
                else{ // 连接套接字事件
                    m_connectionStore[connfd].parse();
                }
            }
        }
    }
}

void Server::set_nonblocking(int sockfd){
    int flags = fcntl(m_listenSockfd, F_GETFL, 0); // 获取sockfd当前的标志
    if(flags < 0){
        perror("调用fcntl(m_listenSockfd, F_GETFL, 0)获取监听套接字标志失败");
        return;
    }

    flags |= O_NONBLOCK;

    if(fcntl(m_listenSockfd, F_SETFL, flags)){
        perror("将监听套接字设置为非阻塞方式失败");
        return;
    }
}