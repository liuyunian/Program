#include <stdio.h>
#include <ucontext.h>
 
void child_func(){
  printf("child\n");
}

void context_test(){
  char stack[1024*128];                     // 128k
  ucontext_t child, main;

  getcontext(&child);                       //获取当前上下文
  child.uc_stack.ss_sp = stack;             //指定栈空间
  child.uc_stack.ss_size = sizeof(stack);   //指定栈空间大小
  child.uc_stack.ss_flags = 0;
  child.uc_link = NULL;                    //设置后继上下文

  makecontext(&child, child_func, 0);            //修改上下文指向func1函数
  swapcontext(&main, &child);                // 切换到child上下文，保存当前上下文到main

  printf("main\n");                         //如果设置了后继上下文，func1函数指向完后会返回此处
}
 
int main(){
  context_test();

  return 0;
}