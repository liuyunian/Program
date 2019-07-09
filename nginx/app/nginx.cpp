#include <stdio.h>
#include <unistd.h>

#include "ngx_c_conf.h"
#include "ngx_func.h"

char ** g_argv;

int main(int argc, char * argv[]){
    g_argv = argv;
    moveEnviron();
    setTitle("nginx: master");

    ConfFileProcessor * confProcessor = ConfFileProcessor::getInstance();
    if(!confProcessor->load("nginx.conf")){
        printf("ERROR: Fail to load config file, exit\n");
        exit(1);
    }

    int port = confProcessor->getItemContent_int("ListenPort", 80);
    printf("ListenPort = %d\n", port);

    const char * str = confProcessor->getItemContent_str("DBInfo"); 
    printf("DBInfo = %s\n", str);

    while(1){
        printf("sleep 1s\n");
        sleep(1);
    }

    freeEnviron();

    return 0;
}


