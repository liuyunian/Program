#include <stdio.h>
#include <unistd.h> // sleep
#include <pthread.h>

__thread int tsd = 0;

void * print(void * arg){
    printf("child thread: tsd = %d\n", tsd);
    sleep(1);

    tsd += 10;
    printf("child thread: tsd = %d\n", tsd);

    return NULL;
}

int main(){
    pthread_t ptid;

    tsd += 5;
    printf("main thread: tsd = %d\n", tsd);
    pthread_create(&ptid, NULL, print, NULL);
    sleep(2);
    printf("main thread: tsd = %d\n", tsd);
    pthread_join(ptid, NULL);

    return 0;
}