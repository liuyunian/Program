#include <iostream>

#include <sys/stat.h>   // stat
#include <errno.h>      // errno
#include <stdio.h>      // perror
#include <unistd.h>     // close
#include <string.h>     // memset
#include <sys/socket.h> // recv send
#include <fcntl.h>      // open
#include <sys/mman.h>
 
#include "c_http_connection.h"

#define MAX_BUF_SZ 2048
#define DEFAULT_KEEP_ALIVE_TIME 10

static std::unordered_map<std::string,std::string> mime = {
    {".html",   "text/html"},
    {".avi",    "video/x-msvideo"},
    {".bmp",    "image/bmp"},
    {".c",      "text/plain"},
    {".doc",    "application/msword"},
    {".gif",    "image/gif"},
    {".gz",     "application/x-gzip"},
    {".htm",    "text/html"},
    {".ico",    "image/x-icon"},
    {".jpg",    "image/jpeg"},
    {".png",    "image/png"},
    {".txt",    "text/plain"},
    {".mp3",    "audio/mp3"},
    {"default", "text/html"}
};

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

    std::cout << m_recvBuf << std::endl;

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
        m_http.method = OTHER_METHOD;
    }

    ret = get_content(content, " "); // 获取http请求行中的url
    if(ret < 0){
        return -1;
    }

    m_http.url = content;

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
        m_http.version = OTHER_VERSION;
    }

    return 0;
}

int Connection::parse_header(){
    /*
        Host: localhost:10000
        Connection: keep-alive
        Cache-Control: max-age=0
        Accept-Language: zh-CN,zh;q=0.9
    */

    /**
     * 如上是http请求头部部分数据，可以看出满足key-value格式，所以这里采用unordered_map来存储
    */
    
    int ret = 0;
    std::string key, value;
    while(m_recvBuf[m_pos] != '\r' && m_recvBuf[m_pos + 1] != '\n'){
        ret = get_content(key, ": ");
		if(ret < 0){
            return -1;
        }

        ret = get_content(value, "\r\n");
		if(ret < 0){
            return -1;
        }

		m_http.header.insert({key, value});
	}

	return 0;
}

void Connection::process_request(){
    // 判断请求方式
    if(m_http.method == GET){
        // 解析URL
    
        struct stat sbuf;
        int ret = stat(m_http.url.c_str(), &sbuf);
        if(ret < 0){
            perror("访问的文件不存在");

            fill_response_header(HTTP_NO_FOUND, nullptr);
        }
        else{
            fill_response_header(HTTP_OK, &sbuf);

            int srcfd=open(m_http.url.c_str(), O_RDONLY, 0);
            if(srcfd < 0){
                perror("打开请求文件失败");
                return;
            }

            char * src_addr = (char *)mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, srcfd, 0);
		    close(srcfd);

            m_sendBuf += std::string(src_addr,src_addr+sbuf.st_size);
            munmap(src_addr,sbuf.st_size);
        }

        std::cout << m_sendBuf << std::endl;

        if(send_msg() < 0){
            perror("发送响应消息失败");
            return;
        }
    }
    else if(m_http.method == POST){
        // ... 暂时不处理
    }
    else{
        // ... 其他请求方式也暂不处理
    }
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

    m_http.url.clear();
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

void Connection::fill_response_header(Http_stateCode stateCode, struct stat * p_sbuf){
    switch(m_http.version){
    case HTTPV1_0:
        m_sendBuf = "HTTP/1.0 ";
        break;
    case HTTPV1_1:
        m_sendBuf = "HTTP/1.1 ";
    default:
        break;
    }

    if(stateCode == HTTP_OK){
        m_sendBuf += "200 OK\r\n";

        std::string fileType;
        int dot_pos = m_http.url.find('.');
        if(std::string::npos == dot_pos){
            fileType = mime["default"];
        }
        else{
            fileType = mime[m_http.url.substr(m_http.url.find('.'))];
        }

        m_sendBuf += "Content-Type: " + fileType + "\r\n";
        m_sendBuf += "Content-Length: " + std::to_string(p_sbuf->st_size) + "\r\n";
    }
    else if(stateCode == HTTP_NO_FOUND){
        m_sendBuf += "404 NO Found\r\n";
        m_sendBuf += "Content-Type: text/html\r\n";
        m_sendBuf += "Content-Length: 0\r\n";
    }

    m_sendBuf += "Server: TestServer\r\n";

    if(m_http.header.find("Connection") != m_http.header.end() 
        && ("Keep-Alive" == m_http.header["Connection"]
        || "keep-alive" == m_http.header["Connection"]))
    {
        m_sendBuf += "Connection: Keep-Alive\r\n";
        m_sendBuf += "Keep-Alive: timeout=" + std::to_string(DEFAULT_KEEP_ALIVE_TIME) + "\r\n";
    }

    m_sendBuf += "\r\n";
}

ssize_t Connection::send_msg(){
    const char * sendBuf = m_sendBuf.c_str();
    size_t bufSize = m_sendBuf.size();
    ssize_t len = 0;
    while(bufSize > 0){
        len = send(m_sockfd, sendBuf, bufSize, 0);
        if(len < 0){
            if(errno == EINTR){
                len = 0;
            }
            else{
                return -1;
            }
        }

        bufSize -= len;
        sendBuf += len;
    }

    return bufSize;
}