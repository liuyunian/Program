#include <stdio.h>
#include <unistd.h>
// #include <signal.h>

int main(){
    printf("nginx进程开始执行\n");
    
    // signal(SIGHUP, SIG_IGN);
    while(1){
        printf("sleep 1s\n");
        sleep(1);
    }
    printf("nginx进程执行结束");
    return 0;
}