#include <vector>

#include <stdio.h>      // perror
#include <stdlib.h>     // exit
#include <unistd.h>     // close
#include <sys/epoll.h>  // epoll相关
#include <errno.h>      // errno
#include <string.h>     // memset
#include <signal.h>     // signal
#include <fcntl.h>      // fcntl
#include <sys/socket.h> // socket相关
#include <arpa/inet.h>  // sockaddr_in

#define LISTEN_PORT 9000
#define BACKLOG 1024
#define BUFFER_SZ 1024

static void set_nonblocking(int sockfd);

int main(){
    /**
     * 忽略SIGPIPE信号
     * SIGPIPE信号发生的实际：如果客户端没有按照四次挥手关闭TCP连接，那么服务器如果调用write或者send发送数据时，会收到一个RST报文段，相应的两个函数错误返回
     * 如果再次调用write或者send发送数据，那么就会收到一个内核发来的SIGPIPE信号，该信号的默认处理动作是终止进程
     * 为了服务器能够长时间稳定原型，这里忽略该信号
    */
    signal(SIGPIPE, SIG_IGN);

    int idlefd = open("/dev/null", O_RDONLY | O_CLOEXEC); // 预留文件描述符

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0){
        perror("创建监听套接字失败");
        exit(1);
    }

    // 设置socket描述符为非阻塞
    set_nonblocking(listenfd);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(LISTEN_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1, ret = 0;
    ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)); // 这里的意义是什么？？
	if(ret < 0){
        perror("设置监听套接字失败");
        exit(1);
    }

    ret = bind(listenfd, (struct sockaddr * )&serv_addr, sizeof(struct sockaddr));
    if(ret < 0){
        perror("监听套接字绑定地址失败");
        exit(1);
    }

    ret = listen(listenfd, BACKLOG);
    if(ret < 0){
        perror("调用listen()函数失败");
        exit(1);
    }

    printf("服务器监听中...\n");

    int epfd = epoll_create1(EPOLL_CLOEXEC);
    if(epfd < 0){
        perror("创建epoll对象失败");
        exit(1);
    }

    struct epoll_event event;
    event.events = EPOLLIN; // 关注监听套接字的读事件，LT工作模式（默认）
    event.data.fd = listenfd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &event);
    if(ret < 0){
        perror("调用epoll_ctl()函数想epoll对象添加监听套接字的读事件 -- 失败");
        exit(1);
    }

    int connfd = -1;
    int nready;                                     // 记录epoll函数返回的事件数
    char buf[BUFFER_SZ] = {0};                      // 应用层的接收和发送缓存
    ssize_t len = 0;                                // 记录recv()函数的返回值
    std::vector<int> clientVec;                     // 保存客户端对应的连接套接字
    std::vector<struct epoll_event> reventVec(16);  // 保存epoll_wait()函数返回的事件，初始大小设置为16，之后可变长
    for(;;){
        nready = epoll_wait(epfd, reventVec.data(), reventVec.size(), -1);
        if(nready <= 0){
            if(errno == EINTR){
                continue;
            }

            perror("epoll_wait()函数执行错误");
            continue;
        }

        if(nready == reventVec.size()){
            reventVec.resize(reventVec.size() * 2);
        }

        for(auto & revent : reventVec){
            if(revent.events & EPOLLIN){ // 发生了可读事件
                if(revent.data.fd == listenfd){
                    connfd = accept(listenfd, (struct sockaddr *)(NULL), NULL);
                    if(connfd < 0){
                        if(errno == EMFILE){
                            /**
                             * 进程描述符已达到上限
                            */

                            close(idlefd); // 关闭预留描述符，进程有了一个空闲描述符
                            connfd = accept(listenfd, (struct sockaddr *)(NULL), NULL); // 此时可以正确的接受连接
                            close(connfd); // 服务器端优雅的关闭连接
                            
                            idlefd = open("/dev/null", O_RDONLY | O_CLOEXEC); // 再次预留文件描述符
                            
                            continue;
                        }

                        perror("accept()函数执行出错");
                        continue;
                    }

                    set_nonblocking(connfd);

                    clientVec.push_back(connfd);

                    event.data.fd = connfd;
                    event.events = EPOLLIN; // 关注连接套接字的读事件，LT工作模式
                    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &event);
                    if(ret < 0){
                        perror("调用epoll_ctl()函数想epoll对象添加连接套接字的读事件 -- 失败");
                        continue;
                    }
                }
                else{
                    connfd = revent.data.fd;
                    len = recv(connfd, buf, BUFFER_SZ, 0);
                    if(len < 0){
                        perror("调用recv()接收数据失败");
                    }
                    else if(len == 0){ // 断开连接
                        printf("客户端断开连接\n");
                        // 从clientVec移除
                        for(auto iter = clientVec.begin(); iter != clientVec.end(); ++ iter){
                            if(*iter == connfd){
                                clientVec.erase(iter);
                                break;
                            }
                        }

                        close(connfd);
                        continue;
                    }

                    send(connfd, buf, strlen(buf), 0);
                    memset(buf, 0, BUFFER_SZ);
                }
            }
        }
    }
}

void set_nonblocking(int sockfd){
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