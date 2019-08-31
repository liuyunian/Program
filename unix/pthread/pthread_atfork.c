/**
 * 多线程中调用fork()创建子进程产生死锁
*/

#include <stdio.h>
#include <unistd.h> // fork getpid sleep
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void * doit(void * arg){
    printf("pid = %d begin doit ...\n", (int)(getpid()));
	pthread_mutex_lock(&mutex);

    sleep(2); // sleep 2s

	pthread_mutex_unlock(&mutex);
    printf("pid = %d end doit ...\n", (int)(getpid()));

    return (void *)0;
}

int main(){
    printf("pid = %d Entering main ...\n", (int)(getpid()));
    pthread_t ptid;
    pthread_create(&ptid, NULL, doit, NULL);

    sleep(1); // sleep 1s

    if(fork() == 0){ // 子进程
        doit(NULL);
    }

    pthread_join(ptid, NULL);
	printf("pid = %d Exiting main ...\n", (int)(getpid()));

    return 0;
}