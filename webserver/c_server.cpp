#include <iostream>

#include <stdio.h> // perror
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>

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

    int on = 1;
    ret = setsockopt(m_listenSockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)); // 这里的意义是什么？？
	if(ret < 0){
        perror("设置监听套接字失败");
        exit(1);
    }

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
                    int connfd = accept(m_listenSockfd, (struct sockaddr * )NULL, NULL);
                    if(connfd < 0){
                        perror("accept()执行失败");
                        continue;
                    }

                    set_nonblocking(connfd);

                    conn = new Connection(connfd);
                    conn->m_event.events = EPOLLIN | EPOLLET;
                    conn->m_event.data.fd = connfd;
                    m_connectionStore.insert({connfd, conn});
                    ret = epoll_ctl(m_epfd, EPOLL_CTL_ADD, connfd, &conn->m_event);
                    if(ret < 0){
                        perror("向epoll对象添加监听套接字事件失败");
                        continue;
                    }

                    std::cout << "一个连接接入" << std::endl;
                }
                else{ // 连接套接字事件
                    ret = m_connectionStore[sockfd]->parse();
                    if(ret < 0){
                        continue;
                    }
                    else if(ret == 0){
                        delete m_connectionStore[sockfd];
                        m_connectionStore.erase(sockfd);
                    }
                    else{
                        m_connectionStore[sockfd]->process_request();
                    }
                }
            }
        }
    }
}

void Server::set_nonblocking(int sockfd){
    int flags = fcntl(sockfd, F_GETFL, 0); // 获取sockfd当前的标志
    if(flags < 0){
        perror("调用fcntl(sockfd, F_GETFL, 0)获取套接字标志失败");
        return;
    }

    flags |= O_NONBLOCK;

    if(fcntl(sockfd, F_SETFL, flags)){
        perror("将套接字设置为非阻塞方式失败");
        return;
    }
}