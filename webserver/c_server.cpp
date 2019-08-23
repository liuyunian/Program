#include <stdio.h> // perror
#include <sys/socket.h>

#include "c_server.h"

Server::Server(int port) : 
    m_listenSockfd(-1),
    m_epfd(-1)
{
    // for(int i = 0; i < MAX_CONN_NUM; ++ i){
        
    // }
}

Server::~Server(){}

void Server::run(){
    m_listenSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(m_listenSockfd < 0){
        perror("Fail to create m_listenSockfd");
        exit(1);
    }

    // 设置socket描述符为非阻塞
    

}