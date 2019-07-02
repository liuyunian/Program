#include <stdio.h>
#include <unistd.h>

#include "ngx_c_conf.h"

int main(int argc, char *const *argv){             
    ConfFileProcessor * confProcessor = ConfFileProcessor::getInstance();
    if(!confProcessor->load("nginx.conf")){
        printf("ERROR: Fail to load config file, exit\n");
        exit(1);
    }

    int port = confProcessor->getItemContent_int("ListenPort", 80);
    printf("ListenPort = %d", port);

    const char * str = confProcessor->getItemContent_str("DBInfo"); 
    printf("DBInfo = %s", str);

    return 0;
}


