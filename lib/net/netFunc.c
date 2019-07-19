#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

void getMacAddress(const char * interface, uint8_t * macAddr){
    struct ifreq ifr;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        // log_fatal(FAT_SYS, "Fail to create socket in getMacAddress()");
        printf("Fail to create socket in getMacAddress()\n");
        return;
    }

    strncpy(ifr.ifr_name, interface, IFNAMSIZ);
    int err = ioctl(sock, SIOCGIFHWADDR, &ifr);
    if(err){
        // log_fatal(FAT_SYS, "Fail to get Mac Addr by ioctl");
        printf("Fail to get Mac Address by ioctl\n");
        return;
    }

    for(int i = 0; i < 6; ++ i){
        macAddr[i] = (uint8_t)ifr.ifr_hwaddr.sa_data[i];
    }
}

char * getIPv4Address(const char * interface){
    struct ifreq ifr;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if(sock < 0){
        // log_fatal(FAT_SYS, "Fail to create socket in getMacAddress()");
        printf("Fail to create socket in getIPv4Address()\n");
        return NULL;
    }
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, interface, IFNAMSIZ);
    int err = ioctl(sock, SIOCGIFADDR, &ifr);
    if(err){
        // log_fatal(FAT_SYS, "Fail to get Mac Addr by ioctl");
        printf("Fail to get IPv4 Address by ioctl\n");
        return NULL;
    }
    
    struct sockaddr_in * sin = (struct sockaddr_in *)&ifr.ifr_addr;
    return inet_ntoa(sin->sin_addr);
}