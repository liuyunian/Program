#ifndef C_SERVER_H_
#define C_SERVER_H_

#include <sys/epoll.h>

#include "c_http_connection.h"

#define MAX_CONN_NUM 1024 // 支持的最大连接数

class Server{
public:
    Server(int port);
    ~Server();

    void run();

private:
    int m_listenSockfd;
    int m_epfd;
    
    epoll_event m_events[MAX_CONN_NUM]; // 单次返回的事件数组
    Connection m_connectionStore[MAX_CONN_NUM]; // http连接对象数组
};

#endif // C_SERVER_H_