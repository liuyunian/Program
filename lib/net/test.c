#include <stdio.h>

#include "netFunc.h"

int main(){
    uint8_t macAddr[6];
    getMacAddress("enp0s31f6", macAddr);

    for(int i = 0; i < 5; ++ i){
        printf("%2x:", macAddr[i]);
    }
    printf("%2x\n", macAddr[5]);

    char * ip = getIPv4Address("enp0s31f6");
    printf("%s\n", ip);
    return 0;
}