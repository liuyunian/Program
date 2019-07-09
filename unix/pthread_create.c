#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

void printID(const char * str){
    pid_t pid;
    pthread_t tid;

    pid = getpid();
    tid = pthread_self();
    printf("%s pid %lu tid %lu (0x%lx)\n", str, (unsigned long)pid, (unsigned long)tid, (unsigned long)tid);
}

void * thread_entry(void * arg){
    printID("new thread: ");
    return (void *)0;
}

int main(){
    pthread_t ntid;
    int ret = pthread_create(&ntid, NULL, thread_entry, NULL);
    if(ret){
        printf("fail to create thread\n");
    }

    printID("main thread: ");
    sleep(1);

    return 0;
}