#include <iostream>

#include <unistd.h>

#include "threadpool_pthread.h"

void * func1(void * arg){
    for(;;){
        std::cout << "test thread func1" << std::endl;
        sleep(1); 
    }

    return (void *)0;
}

void * func2(void * arg){
    for(;;){
        std::cout << "test thread func2" << std::endl;
        sleep(1);
    }

    return (void *)0;
}

int main(){
    ThreadPool * tp = ThreadPool::getInstance();
    tp->threadpool_create();

    tp->threadpool_call(func1, NULL);
    tp->threadpool_call(func2, NULL);

    while(1){}

    tp->threadpool_stop();
    
    return 0;
}