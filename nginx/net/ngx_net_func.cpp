/**
 * 该源文件放置通用的网络函数
 */

#include <sys/ioctl.h>

/**
 * @brief 将socket描述符设置为非阻塞模式
 * @param sockfd要设置的描述符
 * @return 成功执行返回0，否则返回-1
 */
int ngx_set_nonblocking(int sockfd){
    int nb = 1; // 0：清除，1：设置  
    if(ioctl(sockfd, FIONBIO, &nb) < 0){ //FIONBIO：设置/清除非阻塞I/O标记：0：清除，1：设置
        return -1;
    }

    return 0;
}