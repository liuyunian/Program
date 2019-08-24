#ifndef C_HTTP_CONNECTION_H_
#define C_HTTP_CONNECTION_H_

#include <sys/epoll.h>

class Connection{
public:
    Connection(int sockfd);
    ~Connection();

    void parse();

public:
    epoll_event m_event;

private:
    int m_sockfd;
};

#endif // C_HTTP_CONNECTION_H_