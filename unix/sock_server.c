#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h> // socket
#include <arpa/inet.h> // socketaddr_in

#define SERV_PORT 9000

int main(){
    int serv_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(serv_fd < 0){
        printf("Failed to create serv_fd\n");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(serv_fd, (struct sockaddr * )&serv_addr, sizeof(struct sockaddr));
    if(ret < 0){
        printf("Failed to bind serv_fd and serv_addr\n");
        exit(1);
    }

    ret = listen(serv_fd, 32);
    if(ret < 0){
        printf("Failed to listen\n");
        exit(1);
    }

    int client_fd;
    const char * content = "I am server";
    for(;;){
        client_fd = accept(serv_fd, (struct sockaddr * )NULL, NULL);

        // 使用write函数给客户端发数据
        write(client_fd, content, strlen(content));
        
        //只给客户端发送一个信息，然后直接关闭套接字连接；
        close(client_fd); 
    }

    close(serv_fd);

    return 0;
}