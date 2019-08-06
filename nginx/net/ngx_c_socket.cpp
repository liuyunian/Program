#include <string.h> // memset
#include <errno.h> // errno
#include <unistd.h> // close
#include <sys/socket.h> // socket setsockopt
#include <arpa/inet.h> // sockaddr_in
#include <sys/ioctl.h> //ioctl

#include "ngx_func.h"
#include "ngx_macro.h"
#include "ngx_c_conf.h"
#include "ngx_c_socket.h"

Socket::Socket() : 
    m_portCount(0)
    {}

Socket::~Socket(){}

bool Socket::ngx_sockets_init(){
    ConfFileProcessor * confProcess = ConfFileProcessor::getInstance();
    m_portCount = confProcess->getItemContent_int("PortCount", NGX_PROT_COUNT);

    int sockfd;
    struct sockaddr_in serv_addr;
    int port;
    char tmp_str[100];

    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    for(int i = 1; i <= m_portCount; ++ i){
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0){
            log(NGX_LOG_CRIT, errno, "Socket::ngx_open_listening_sockets()中执行socket()失败, i = %d", i);
            return false;
        }

        //@setsockopt()：设置一些套接字参数选项
        //参数2：是表示级别，和参数3配套使用，也就是说，参数3如果确定了，参数2就确定了;
        //参数3：允许重用本地地址
        //设置 SO_REUSEADDR，目的第五章第三节讲解的非常清楚：主要是解决TIME_WAIT这个状态导致bind()失败的问题
        int reuseaddr = 1;  //1:打开对应的设置项
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&reuseaddr, sizeof(reuseaddr)) < 0){
            log(NGX_LOG_CRIT, errno, "Socket::ngx_open_listening_sockets()中执行setsockopt(SO_REUSEADDR)失败, i = %d", i);
            close(sockfd);                                               
            return false;
        }

        if(!ngx_set_nonblocking(sockfd)){
            log(NGX_LOG_ERR, errno, "Socket::ngx_open_listening_sockets()中执行ngx_set_nonblocking()失败, i = %d", i);
            close(sockfd);                                               
            return false;
        }

        memset(tmp_str, 0, 100);
        sprintf(tmp_str, "Port%d", i);
        port = confProcess->getItemContent_int(tmp_str);
        if(port < 0){
            if(m_portCount == 1){
                port = NGX_LISTEN_PORT;
            }
            else{
                log(NGX_LOG_ERR, 0, "配置文件中没有提供Port%d", i);
                close(sockfd);                                               
                return false;
            }
        }

        serv_addr.sin_port = htons(in_port_t(port));

        if(bind(sockfd, (struct sockaddr * )&serv_addr, sizeof(serv_addr)) < 0){
            log(NGX_LOG_CRIT, errno, "Socket::ngx_open_listening_sockets()中执行bind()失败, i = %d", i);
            close(sockfd);                                               
            return false;
        }

        if(listen(sockfd, NGX_LISTEN_BACKLOG) < 0){
            log(NGX_LOG_CRIT, errno, "Socket::ngx_open_listening_sockets()中执行listen()失败, i = %d", i);
            close(sockfd);                                               
            return false;
        }

        log(NGX_LOG_INFO, 0, "监听%d端口成功", port);
        m_listenSokcetStore.insert({port, sockfd});
    }

    return true;
}

void Socket::ngx_close_listening_sockets(){
    int port;
    for(auto & item : m_listenSokcetStore){
        port = item.second;
        close(port);
        log(NGX_LOG_INFO, 0, "监听端口%d已关闭", port);
    }
}

bool Socket::ngx_set_nonblocking(int sockfd){
    int nb=1; // 0：清除，1：设置  
    if(ioctl(sockfd, FIONBIO, &nb) == -1){ //FIONBIO：设置/清除非阻塞I/O标记：0：清除，1：设置
        return false;
    }

    return true;

    // 如下也是一种写法，跟上边这种写法其实是一样的，但上边的写法更简单
    /* 
    // fcntl:file control【文件控制】相关函数，执行各种描述符控制操作
    // 参数1：所要设置的描述符，这里是套接字【也是描述符的一种】
    int opts = fcntl(sockfd, F_GETFL);  //用F_GETFL先获取描述符的一些标志信息
    if(opts < 0){
        ngx_log_stderr(errno,"CSocekt::setnonblocking()中fcntl(F_GETFL)失败.");
        return false;
    }
    opts |= O_NONBLOCK; //把非阻塞标记加到原来的标记上，标记这是个非阻塞套接字【如何关闭非阻塞呢？opts &= ~O_NONBLOCK,然后再F_SETFL一下即可】
    if(fcntl(sockfd, F_SETFL, opts) < 0){
        ngx_log_stderr(errno,"CSocekt::setnonblocking()中fcntl(F_SETFL)失败.");
        return false;
    }
    return true;
    */
}