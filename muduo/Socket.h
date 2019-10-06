#ifndef SOCKET_H_
#define SOCKET_H_

#include <tools_cxx/noncopyable.h>

class Socket : public noncopyable {
public:
    Socket(int sockfd);

    ~Socket();

    inline int get_sockfd(){
        return m_sockfd;
    }

    

private:

private:
    const int m_sockfd;
};

#endif // SOCKET_H_