/**
 * pthread_exit、pthread_join的用法示例
 */

#include <stdio.h> // printf
#include <pthread.h> // pthread_exit pthread_join
#include <string.h> // strerror
#include <stdlib.h> // exit

void * func1(void * arg){
    printf("thread 1 returning\n");
    return (void *)1;
}

void * func2(void * arg){
    printf("thread 2 exitint\n");
    pthread_exit((void *)2);
}

int main(){
    pthread_t tid1, tid2;
    int err = -1;
    void * t_rtn;

    err = pthread_create(&tid1, NULL, &func1, NULL);
    if(err != 0){
        printf("Failed to create thread 1, %s\n", strerror(err));
        exit(1);
    }

    err = pthread_join(tid1, &t_rtn);
    if(err != 0){
        printf("can't join with thread 1, %s\n", strerror(err));
        exit(1);
    }

    printf("thread 1 exit code %d\n", (int)t_rtn);

    // ----------------------------------------------------------------

    err = pthread_create(&tid2, NULL, &func2, NULL);
    if(err != 0){
        printf("Failed to create thread 2, %s\n", strerror(err));
        exit(1);
    }

    err = pthread_join(tid2, &t_rtn);
    if(err != 0){
        printf("can't join with thread 2, %s\n", strerror(err));
        exit(1);
    }

    printf("thread 2 exit code %d\n", (int)t_rtn);

    return 0;
}