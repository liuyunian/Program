#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int create_daemon(){
    pid_t pid = fork();
    if(pid < 0){
        return -1;
    }
    else if(pid == 0){
        int ret;

        ret = setsid();
        if(ret < 0){
            return -1;
        }

        umask(0);

        int fd = open("/dev/null", O_RDWR);
        if(fd == -1){
            return -1;
        }

        ret = dup2(fd, STDIN_FILENO);
        if(ret < 0){
            return -1;
        }

        ret = dup2(fd, STDOUT_FILENO);
        if(ret < 0){
            return -1;
        }

        if(fd > STDERR_FILENO){
            ret = close(fd);
            if(ret < 0){ // 释放资源
                return -1;
            }
        }

        return 0;
    }
    else{ // 父进程退出
        exit(0);
    }
}

int main(){
    if(create_daemon() < 0){
        exit(1);
    }

    while(1){
        sleep(1);
    }

    return 0;
}