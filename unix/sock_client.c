#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 9000
#define IP "10.56.75.87"

int main(){
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd < 0){
        printf("Failed to create socket\n");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if(inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0){
        printf("Failed to exec inet_pton()\n");
        exit(1);
    }

    if(connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("Failed to connect to server\n");
        exit(1);
    }

    char recv_buf[1024] = {0};
    while(read(client_fd, recv_buf, 1024) > 0){
        printf("receive: %s\n", recv_buf);
    }

    close(client_fd);
    return 0;
}