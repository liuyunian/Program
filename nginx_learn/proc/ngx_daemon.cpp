#include <unistd.h> // fork setsid close
#include <fcntl.h> // open dup2 
#include <sys/stat.h> // umask
#include <errno.h> // errno

#include "app/ngx_log.h"
#include "_include/ngx_func.h"

int ngx_create_daemon(){
    pid_t pid = fork();
    if(pid < 0){ // 执行出错
        ngx_log(NGX_LOG_FATAL, errno, "ngx_create_daemon函数中fork()失败");
        return -1;
    }
    else if(pid == 0){ // 子进程
        int ret;

        ret = setsid();
        if(ret < 0){
            ngx_log(NGX_LOG_ERR, errno, "ngx_create_daemon函数中setsid()失败");
            return -1;
        }

        umask(0);

        int fd = open("/dev/null", O_RDWR);
        if(fd == -1){
            ngx_log(NGX_LOG_ERR, errno, "ngx_create_daemon函数中open(\"/dev/null\")失败");
            return -1;
        }

        ret = dup2(fd, STDIN_FILENO);
        if(ret < 0){
            ngx_log(NGX_LOG_ERR, errno, "ngx_create_daemon函数中dup2(STDIN_FILENO)失败");
            return -1;
        }

        ret = dup2(fd, STDOUT_FILENO);
        if(ret < 0){
            ngx_log(NGX_LOG_ERR, errno, "ngx_create_daemon函数中dup2(STDOUT_FILENO)失败");
            return -1;
        }

        if(fd > STDERR_FILENO){
            ret = close(fd);
            if(ret < 0){ // 释放资源
                ngx_log(NGX_LOG_FATAL, errno, "ngx_create_daemon函数中close()失败");
                return -1;
            }
        }

        return 0;
    }
    else{ // 父进程
        return 1;
    }
}