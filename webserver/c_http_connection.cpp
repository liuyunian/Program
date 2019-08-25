#include <errno.h>  // errno
#include <stdio.h>  // perror

#include "c_http_connection.h"

#define MAX_BUF_SZ 2048

Connection::Connection(int sockfd) : 
    m_sockfd(sockfd){}

Connection::~Connection(){}

int Connection::parse(){
    clear();

    ssize_t len = recv_msg();
    if(len < 0){
        return -1;
    }
    else if(len == 0){
        return 0;
    }

    int ret = parse_request();
    if(ret < 0){
        return -1;
    }

    ret = parse_header();
    if(ret < 0){
        return -1;
    }

    return 1; // 正常情况
}

int Connection::parse_request(){
    int ret = 0;
    std::string content;

    ret = get_content(content, " /"); // 获取http请求行中的method
    if(ret < 0){
        return -1;
    }

    if(content == "GET"){
        m_http.method = GET;
    }
    else if(content == "POST"){
        m_http.method = POST;
    }
    else{
        m_http.method = OTHER;
    }

    ret = get_content(m_http.url, " "); // 获取http请求行中的url
    if(ret < 0){
        return -1;
    }

    ret = get_content(content, "\r\n"); // 获取http请求行中的url
    if(ret < 0){
        return -1;
    }

    if(content == "HTTP/1.0"){
        m_http.version = HTTPV1_0;
    }
    else if(content == "HTTP/1.1"){
        m_http.version = HTTPV1_1;
    }
    else{
        m_http.version = OTHER;
    }

    return 0;
}

int Connection::parse_header(){

}

int Connection::get_content(std::string & content, std::string sepa){
    int index = m_recvBuf.find(sepa, m_pos);
    if(index == std::string::npos){ // 没有找到指定的分隔符
        return -1;
    }

    content = m_recvBuf.substr(m_pos, index - m_pos);
    m_pos = index + sepa.size();

    return 0;
}

void Connection::clear(){
    m_recvBuf.clear();
    m_sendBuf.clear();

    m_pos = 0;

    memset(&m_http, 0, sizof(HttpProtocol));
}

ssize_t Connection::recv_msg(){
    ssize_t len = 0;
    ssize_t recvSize = 0;
    char buf[MAX_BUF_SZ];
    for(;;){
        len = recv(m_sockfd, buf, MAX_BUF_SZ, 0);
        if(len < 0){
            if(errno == EINTR){ // 信号中断产生的错误
                continue;
            }
            else if(errno == EAGAIN){
                return recvSize;
            }
            else{
                perror("接收数据出错");
                return -1;
            }
        }
        else if(len == 0){ // 连接断开
            return 0;
        }

        recvSize += len;
        m_recvBuf += std::string(buf, buf + len);
    }
}

ssize_t Connection::send_msg(){}