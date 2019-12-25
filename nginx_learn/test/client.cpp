#include <iostream>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <app/ngx_log.h>
#include <app/ngx_c_conf.h>

#include "format.h"

struct LogInfor g_logInfor;

void init(){
    // 加载配置文件
    ConfFileProcessor * cfp = ConfFileProcessor::getInstance();
    if(!cfp->ngx_conf_load("client.conf")){
        std::cout << "Fail to load client.conf, exit" << std::endl;
        exit(1);
    }

    // 初始化日志
    ngx_log_init();
}

int connect_server(){
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd < 0){
        ngx_log(NGX_LOG_FATAL, errno, "在socket_init()函数中执行socket()函数出错");
        return -1;
    }

    ConfFileProcessor * cfp = ConfFileProcessor::getInstance();
    const char * IPaddr = cfp->ngx_conf_getContent_str("IP");
    int port = cfp->ngx_conf_getContent_int("Port");

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, IPaddr, &serv_addr.sin_addr) <= 0){ // IP地址转换
        ngx_log(NGX_LOG_FATAL, errno, "在socket_init()函数中执行inet_pton()函数出错");
        close(clientfd);
        return -1;
    }

    if(connect(clientfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        ngx_log(NGX_LOG_FATAL, errno, "在socket_init()函数中执行connect()函数出错");
        close(clientfd);
        return -1;
    }

    return clientfd;
}

// void padding_pkt(uint8_t * buf){

// }

int main(){
    init();

    int sockfd = connect_server();
    if(sockfd < 0){
        ngx_log(NGX_LOG_ERR, 0, "连接服务器失败！");

        // 关闭日志文件
        ngx_log_close();

        exit(1);
    }

    ngx_log(NGX_LOG_INFO, 0, "连接成功！");

    uint8_t sendBuf[1024] = {0};
    uint8_t * p_buf = sendBuf;

    PktHeader * pktHdr = (PktHeader *)p_buf;
    pktHdr->len = htons(sizeof(PktHeader));
    pktHdr->type = htons(6);
    pktHdr->crc32 = 0;      

    int len = send(sockfd, sendBuf, sizeof(PktHeader), 0);
    if(len < sizeof(PktHeader)){
        ngx_log(NGX_LOG_ERR, errno, "数据发送失败！");
    }

    while(1){}

    close(sockfd);

    return 0;
}