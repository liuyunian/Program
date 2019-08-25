#ifndef C_SERVER_H_
#define C_SERVER_H_

#include <map>

#include <sys/epoll.h>

#include "c_http_connection.h"

#define MAX_CONN_NUM 1024 // 支持的最大连接数

#define BACKLOG 1024 // 用于指定listen第二个参数

class Server{
public:
    Server(int port);
    ~Server();

    int run();

private:
    void set_nonblocking(int sockfd);

private:
    int m_port;
    int m_listenSockfd;
    int m_epfd;
    
    epoll_event m_events[MAX_CONN_NUM]; // 单次返回的事件数组
    std::map<int, Connection *> m_connectionStore; // http连接对象数组
};

#endif // C_SERVER_H_