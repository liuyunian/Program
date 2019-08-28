#include <stdio.h>      // perror
#include <string.h>     // memset
#include <stdlib.h>     // exit
#include <unistd.h>     // close
#include <sys/socket.h> // socket
#include <arpa/inet.h>  // socketaddr_in

#define LISTEN_PORT 9000
#define BACKLOG 1024
#define BUFFER_SZ 1024

int main(){
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0){
        perror("创建监听套接字失败");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(LISTEN_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(listenfd, (struct sockaddr * )&serv_addr, sizeof(struct sockaddr));
    if(ret < 0){
        perror("监听套接字绑定地址失败");
        exit(1);
    }

    ret = listen(listenfd, 32);
    if(ret < 0){
        perror("调用listen()函数失败");
        exit(1);
    }

    char buf[BUFFER_SZ];
    ssize_t len = 0;
    int connfd = -1;
    for(;;){
        connfd = accept(listenfd, (struct sockaddr * )NULL, NULL);
        if(connfd < 0){
            perror("accept函数执行出错");
            exit(1);
        }

        while((len = recv(connfd, buf, BUFFER_SZ, 0)) > 0){
            // 使用write函数给客户端发数据
            send(connfd, buf, strlen(buf), 0);
            memset(buf, 0, BUFFER_SZ);
        }

        close(connfd);
    }

    close(listenfd);
    return 0;
}