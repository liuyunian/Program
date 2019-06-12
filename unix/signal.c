#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void onSignal(int sig){
    if(sig == SIGUSR1){
        printf("收到了SIGUSR1信号\n");
    }
    else if(sig == SIGUSR2){
        printf("收到了SIGUSR2信号\n");
    }
}

int main(){
    if(signal(SIGUSR1, onSignal) == SIG_ERR){
        printf("无法捕捉SIGUSR1信号\n");
    }
    else if(signal(SIGUSR2, onSignal) == SIG_ERR){
        printf("无法捕捉SIGUSR2信号\n");
    }

    while(1){
        printf("sleep 1s\n");
        sleep(1);
    }
    return 0;
}