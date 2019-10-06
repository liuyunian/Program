#ifndef SOCKETSOPS_H_
#define SOCKETSOPS_H_

#include <arpa/inet.h>

namespace sockets {
// core
int create_socket(int family);

int create_nooblocking_socket(int family);

ssize_t read(int sockfd, void * buf, ssize_t count);

ssize_t write(int sockfd, const void * buf, ssize_t count);

void close(int sockfd);

// server
void bind(int sockfd, const struct sockaddr* addr);

void listen(int sockfd);

int accept(int sockfd, struct sockaddr* addr);

// client
void connect(int sockfd, const struct sockaddr* addr);

// other
void to_ip(const struct sockaddr* addr, char* buf, size_t size);

void to_ip_and_port(const struct sockaddr* addr, char* buf, size_t size);

void from_ip_and_port(const char * ip, uint16_t port, struct sockaddr_in* addr);

void from_ip_and_port(const char * ip, uint16_t port, struct sockaddr_in6* addr);
}

#endif // SOCKETSOPS_H_