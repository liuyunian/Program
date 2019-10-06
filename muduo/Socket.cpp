#include "Socket.h"
#include "SocketsOps"

Socket::Socket(int sockfd) :
    m_sockfd(sockfd){}

Socket::~Socket(){
    sockets::close(m_sockfd);
}

