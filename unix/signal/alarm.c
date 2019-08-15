#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void startTimer(int time, void (*handler)(int)){
    signal(SIGALRM, handler);
    
    alarm(time);
}

void cancalTimer(){
    alarm(0);
}

void timeEvent(){
    printf("on time\n");
}

int main(){
    startTimer(5, timeEvent);

    while(1){
        printf("sleep 1s\n");
        sleep(1);
        cancalTimer();
    }

    return 0;
}