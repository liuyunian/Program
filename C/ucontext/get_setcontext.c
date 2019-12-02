
#include <stdio.h>
#include <ucontext.h> // ucontext_t getcontext setcontext
#include <unistd.h>   // sleep
 
int main(int argc, const char *argv[]){
  ucontext_t context;

  getcontext(&context);
  printf("Hello world\n");
  sleep(1);
  setcontext(&context);

  return 0;
}

// 执行结果：会一直不断的输出Hello world
