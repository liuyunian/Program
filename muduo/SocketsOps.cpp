#include <sys/socket.h>
#include <fcntl.h>      // fcntl
#include <errrno>       // errno
#include <unistd.h>     // close
#include <string.h>     // strlen

#include <tools_cxx/log.h>

#include "SocketsOps.h"
#include "Endian.h"

static void
set_nonblock_and_cloexec(int sockfd){
    // non-block
    int flags = ::fcntl(sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    int ret = ::fcntl(sockfd, F_SETFL, flags);
    if(ret < 0){
        LOG_SYSFATAL("Failed to set nonblocking in set_nonblock_and_cloexec(int)");
    }

    // cloexec
    flags = ::fcntl(sockfd, F_GETFL, 0);
    flags |= FD_CLOEXEC;
    ret = ::fcntl(sockfd, F_SETFL, flags);
    if(ret < 0){
        LOG_SYSFATAL("Failed to set cloexec in set_nonblock_and_cloexec(int)");
    }
}

int sockets::create_socket(int family){
    int sockfd = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
    if(sockfd < 0){
        LOG_SYSFATAL("Failed to create socket in create_socket(int)");
    }

    return sockfd;
}

int sockets::create_nonblocking_socket(int family){
#ifdef __linux__
    int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if(sockfd < 0){
        LOG_SYSFATAL("Failed to create socket in create_nonblocking_socket(int)");
    }
#else
    int sockfd = create_socket(family);
    set_nonblock_and_cloexec(sockfd);
#endif

    return sockfd;
}

ssize_t sockets::read(int sockfd, void * buf, ssize_t count){
    return ::read(sockfd, buf, count);
}

ssize_t sockets::write(int sockfd, const void * buf, ssize_t count){
    return ::write(sockfd, buf, count);
}

void sockets::close(int sockfd){
    int ret = ::close(sockfd);
    if(ret < 0){
        LOG_SYSFATAL("Failed to close sockfd in sockets::close(int)");
    }
}

void sockets::bind(int sockfd, const struct sockaddr* addr){
    int ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
    if(ret < 0){
        LOG_SYSFATAL("Failed to bind addr in sockets::bind(int, const struct sockaddr*)");
    }
}

void sockets::listen(int sockfd){
    int ret = ::listen(sockfd, SOMAXCONN);
    if(ret < 0){
        LOG_SYSFATAL("Failed to listen in sockets::listen(int)");
    }
}

int sockets::accept(int sockfd, struct sockaddr* addr){
    socklen_t addrlen = static_cast<socklen_t>(sizeof(struct sockaddr_in6));
#ifdef __linux__
    int connfd = accept4(sockfd, addr, &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
#else
    int connfd = accept(sockfd, addr, &addrlen);
    set_nonblock_and_cloexec(connfd);
#endif
    if(connfd < 0){
        switch(errno){
            case EAGAIN:
            case ECONNABORTED:
            case EINTR:
            case EPROTO:
            case EPERM:
            case EMFILE:
                // expected errors
                LOG_SYSERR("expected error of ::accept");
                break;
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENFILE:
            case ENOBUFS:
            case ENOMEM:
            case ENOTSOCK:
            case EOPNOTSUPP:
                // unexpected errors
                LOG_SYSFATAL("unexpected error of ::accept");
                break;
            default:
                LOG_SYSFATAL("unknown error of ::accept ");
                break;
        }
    }

    return connfd;
}

void sockets::connect(int sockfd, const struct sockaddr* addr){
    int ret = ::connect(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
    if(ret < 0){
        LOG_SYSFATAL("Failed to connect server in sockets::connect(int, const struct sockaddr*)");
    }
}

void to_ip(const struct sockaddr* addr, char* buf, size_t size){
    if(addr->sa_family == AF_INET){
        assert(size >= INET_ADDRSTRLEN);
        const struct sockaddr_in* addr4 = static_cast<struct socketaddr_in*>(addr);
        ::inet_ntop(AF_INET, &addr4->sin_addr, buf, size);
    }
    else{
        assert(size >= INET6_ADDRSTRLEN);
        const struct sockaddr_in6* addr6 = static_cast<struct socketaddr_in6*>(addr);
        ::inet_ntop(AF_INET6, &addr6->sin6_addr, buf, size);
    }
}

void to_ip_and_port(const struct sockaddr* addr, char* buf, size_t size){
    to_ip(addr, buf, size);
    size_t end = ::strlen(buf);
    const struct sockaddr_in* addr4 = static_cast<struct socketaddr_in*>(addr);
    uint16_t port = sockets::network_to_host16(addr4->sin_port);
    assert(size > end);
    snprintf(buf + end, size - end, ":%u", port);
}

void from_ip_and_port(const char * ip, uint16_t port, struct sockaddr_in* addr){
    addr->sin_family = AF_INET;
    addr->sin_port = host_to_network16(port);
    int ret = ::inet_pton(AF_INET, ip, &addr->sin_addr);
    if(ret < 0){
        LOG_SYSFATAL("Failed to call inet_pton in from_ip_and_port");
    }
}

void from_ip_and_port(const char * ip, uint16_t port, struct sockaddr_in6* addr){
    addr->sin_family = AF_INET6;
    addr->sin_port = host_to_network16(port);
    int ret = ::inet_pton(AF_INET6, ip, &addr->sin6_addr);
    if(ret < 0){
        LOG_SYSFATAL("Failed to call inet_pton in from_ip_and_port");
    }
}