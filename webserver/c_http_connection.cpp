#include "c_http_connection.h"

Connection::Connection(int sockfd) : 
    m_sockfd(sockfd){}

Connection::~Connection(){}

void Connection::parse(){}