#include <stdio.h>
#include <signal.h>
#include <stdlib.h> // exit
#include <unistd.h> // sleep

void sig_quit(int signo){
    printf("收到了SIGQUIT信号!\n");
}

int main(){
    signal(SIGQUIT, sig_quit);                  // 注册SIGQUIT信号处理函数，SIGQUIT信号可以通过Ctrl+\触发

    sigset_t newSet, oldSet;                    // 定义两个信号集变量
    sigemptyset(&newSet);                       // 清空信号集，全部位置0
    sigaddset(&newSet, SIGQUIT);                // 将SIGQUIT对应的位置1，表示要屏蔽SIGQUIT信号

    sigprocmask(SIG_BLOCK, &newSet, &oldSet);
    printf("进程关联的信号集设置为newSet\n");

    if(sigismember(&newSet, SIGQUIT)){
        printf("SIGQUIT信号被屏蔽了，按ctrl+\\测试\n");
    }

    printf("sleep 10s\n");
    sleep(10);
    printf("sleep end\n");

    sigprocmask(SIG_SETMASK, &oldSet, NULL);
    printf("进程关联的信号集重新设置为oldSet\n");

    if(sigismember(&oldSet, SIGQUIT)){
        printf("此时SIGQUIT信号还是被屏蔽的!\n");
        exit(0);
    }
    
    printf("SIGQUIT信号没有被屏蔽，按ctrl+\\测试\n");
    printf("sleep 10s\n");
    sleep(10);


    return 0;
}