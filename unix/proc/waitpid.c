#include <stdio.h>
#include <sys/wait.h> // waitpid
#include <unistd.h>  //fork
#include <stdlib.h>
#include <signal.h>

void sig_usr(int signo)
{
    printf("收到了SIGCHLD信号，进程id=%d!\n",getpid());    

    int status;
    pid_t pid = waitpid(-1,&status,WNOHANG); 
    if(pid == 0){
        printf("WARNNING: 子进程还没有终止\n");
    }
    else if(pid == -1){
        printf("ERROR: waitpid error\n");
    }
    else{
        printf("INFO: 终止状态为: %d\n", status);
    }
}

int main(){
    if(signal(SIGCHLD,sig_usr) == SIG_ERR){
        printf("ERROR: 无法捕捉SIGCHLD信号!\n");
        exit(1);
    }

    pid_t pid = fork(); 

    if(pid < 0)
    {
        printf("子进程创建失败，很遗憾!\n");
        exit(1);
    }

    for(;;)
    {        
        sleep(1); //休息1秒
        printf("休息1秒，进程id=%d!\n",getpid());
    }

    return 0;
}