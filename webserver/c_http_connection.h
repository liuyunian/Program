#ifndef C_HTTP_CONNECTION_H_
#define C_HTTP_CONNECTION_H_

#include <string>
#include <unordered_map>

#include <sys/epoll.h>

enum Http_method{
    GET,
    POST,
    OTHER_METHOD
};

enum Http_version{
    HTTPV1_0,
    HTTPV1_1,
    OTHER_VERSION
};

enum Http_stateCode{
    HTTP_OK = 200,
    HTTP_NO_FOUND = 404
};

class Connection{
public:
    Connection(int sockfd);
    ~Connection();

    int parse();

    void process_request();

public:
    epoll_event m_event;

private:
    void clear();

    ssize_t recv_msg();

    ssize_t send_msg();

    int parse_request();

    int parse_header();

    int get_content(std::string & content, std::string sepa);

    void fill_response_header(Http_stateCode stateCode, struct stat * p_sbuf);

    struct HttpProtocol{
        Http_method method;
        std::string url;
        Http_version version;
        std::unordered_map<std::string, std::string> header;
    };

private:
    int m_sockfd;

    std::string m_recvBuf;
    std::string m_sendBuf;

    HttpProtocol m_http;
    int m_pos; // 记录解析http协议的位置
};

#endif // C_HTTP_CONNECTION_H_