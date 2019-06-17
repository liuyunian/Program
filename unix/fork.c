#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    printf("father process is running, pid = %d\n", getpid());

    pid_t pid;
    pid = fork();
    if(pid == -1){
        printf("ERROR: fork error\n");
        exit(1);
    }
    else if(pid == 0){
        printf("child process is running, pid = %d\n", getpid());
    }
    else{
        printf("father process is running too, pid = %d\n", getpid());
    }

    return 0;
}