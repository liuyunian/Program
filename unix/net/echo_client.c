#include <stdio.h>      // perror
#include <unistd.h>     // close
#include <string.h>     // memset
#include <stdlib.h>     // exit
#include <sys/socket.h> // socket ...
#include <arpa/inet.h>  // sockaddr

#define PORT 9000
#define IP "127.0.0.1"
#define BUFFER_SZ 1024

int main(){
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd < 0){
        perror("创建套接字失败");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if(inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0){ // IP地址转换
        perror("IP地址转换失败");
        exit(1);
    }

    if(connect(clientfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        perror("连接服务器失败");
        exit(1);
    }

    char recvBuf[BUFFER_SZ] = {0};
    char sendBuf[BUFFER_SZ] = {0};
    while (fgets(sendBuf, sizeof(sendBuf), stdin) != NULL){
		send(clientfd, sendBuf, strlen(sendBuf), 0);
		recv(clientfd, recvBuf, sizeof(recvBuf), 0);

		fputs(recvBuf, stdout);
		memset(sendBuf, 0, BUFFER_SZ);
		memset(recvBuf, 0, BUFFER_SZ);
	}

    close(clientfd);
    return 0;
}