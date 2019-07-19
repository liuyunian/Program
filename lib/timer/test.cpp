#include <iostream>

#include <unistd.h>

#include "timer.h"

void timerEvent(){
    std::cout << "on time" << std::endl;
}

int main(){
    Timer * timer = new Timer();
    // timer->asyncOnce(5000, timerEvent);
    timer->asyncLoop(5000, timerEvent);

    int flag = 0;
    while(1){
        std::cout << "sleep 1s" << std::endl;
        sleep(1);

        ++ flag;
        if(flag == 13){
            timer->cancelTimer();
        }
        // if(!timer->asyncOnce(5000, timerEvent)){
        //     std::cout << "false" << std::endl;
        // }
        // else{
        //     std::cout << "true" << std::endl;
        // }
    }

    delete timer;

    return 0;
}