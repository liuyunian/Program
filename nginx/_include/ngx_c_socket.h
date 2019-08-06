#ifndef NGX_C_SOCKET_H_
#define NGX_C_SOCKET_H_

#include <map>

class Socket{
public:
    Socket();
    ~Socket();

    bool ngx_sockets_init(); // 打开监听套接字

	void ngx_close_listening_sockets(); // 关闭监听套接字

private:
	bool ngx_set_nonblocking(int sockfd); // 将套接字设置为非阻塞

private:
    int m_portCount; // 监听端口的数目
    int m_epfd;

    std::map<int, int> m_listenSokcetStore;
};

#endif // NGX_C_SOCKET_H_