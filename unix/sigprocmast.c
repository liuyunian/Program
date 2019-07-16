#include <stdio.h>
#include <signal.h>
#include <stdlib.h> // exit
#include <unistd.h> // sleep

void sig_quit(int signo){
    printf("收到了SIGQUIT信号!\n");

    if(signal(SIGQUIT,SIG_DFL) == SIG_ERR){
        printf("无法为SIGQUIT信号设置缺省处理(终止进程)!\n");
        exit(1);
    }
}

int main(){
    if(signal(SIGQUIT, sig_quit) == SIG_ERR){ // 注册SIGQUIT信号处理函数
        printf("无法捕获SIGQUIT信号\n");
        exit(1);
    }

    sigset_t newMask, oldMask; // 定义两个信号集变量
    sigemptyset(&newMask); // 清空信号集，全部位置1
    sigaddset(&newMask, SIGQUIT); // 将SIGQUIT对应的位置1，表示要屏蔽SIGQUIT信号

    int ret = sigprocmask(SIG_BLOCK, &newMask, &oldMask);
    if(ret < 0){
        printf("sigprocmask(SIG_BLOCK)失败!\n");
        exit(1);
    }
    else{
        printf("进程关联的信号集设置为newMask\n");
    }

    printf("我要开始休息10秒了--------begin--，此时我无法接收SIGQUIT信号!\n");
    sleep(10);
    printf("我已经休息了10秒了--------end----!\n");

    if(sigismember(&newMask,SIGQUIT)){
        printf("SIGQUIT信号被屏蔽了!\n");
    }
    else{
        printf("SIGQUIT信号没有被屏蔽!!!!!!\n");
    }

    ret = sigprocmask(SIG_SETMASK, &oldMask, NULL);
    if(ret < 0){
        printf("sigprocmask(SIG_SETMASK)失败!\n");
        exit(1);
    }
    else{
        printf("进程关联的信号集重新设置为oldMask\n");
    }

    if(sigismember(&oldMask,SIGQUIT)){
        printf("SIGQUIT信号被屏蔽了!\n");
    }
    else{
        printf("SIGQUIT信号没有被屏蔽，您可以发送SIGQUIT信号了，我要sleep(10)秒钟!!!!!!\n");
        int mysl = sleep(10);
        if(mysl > 0)
        {
            printf("sleep还没睡够，剩余%d秒\n",mysl);
        }
    }
    printf("再见了!\n");

    return 0;
}