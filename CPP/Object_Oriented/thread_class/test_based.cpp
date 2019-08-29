#include <iostream>
#include <functional>

#include <unistd.h>

#include "thread_based.h"

void print(){
    std::cout << "this is a thread" << std::endl;
}

void print2(int count){
    while(count > 0){
        std::cout << "this is a thread " << count << std::endl;
        sleep(1);
        -- count;
    }
}

int main(){
    // Thread t(print);
    // t.start();
    // t.join();

    Thread t2(std::bind(print2, 5));
    t2.start();
    t2.join();

    return 0;
}