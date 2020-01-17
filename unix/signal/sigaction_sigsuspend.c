#include <stdio.h>  // printf
#include <string.h> // memset
#include <signal.h> // signal 

void handler_sigusr1(int sig){
  printf("收到了SIGUSR1信号\n");  
}

int main(){
  sigset_t set;
  sigemptyset(&set);

  struct sigaction act;
  memset(&act, 0, sizeof(act));
  act.sa_handler = handler_sigusr1;
  act.sa_mask = set;

  if(sigaction(SIGUSR1, &act, NULL) < 0){
    perror("sigaction error");
  }

  while(1){
    sigsuspend(&set);
  }

  return 0;
}