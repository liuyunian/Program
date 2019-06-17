#include <stdio.h>
#include <pthread.h>

int main(){
    pthread_t tid = pthread_self();

    printf("thread id = %ld\n", tid->__sig);
    return 0;
}