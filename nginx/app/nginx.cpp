#include <unistd.h>
#include <stdio.h>

#include "ngx_c_conf.h"
#include "ngx_func.h"
#include "ngx_macro.h"

static void freeSource();

char ** g_argv;

int main(int argc, char * argv[]){
    g_argv = argv;
    int exitCode = 0;

    ConfFileProcessor * confProcessor = ConfFileProcessor::getInstance();
    if(!confProcessor->load("nginx.conf")){
        log_stderr(NGX_LOG_ERR, 0, "Fail to load %s, exit\n", "nginx.conf");
        exitCode = 2;
        goto exit_label; // 不能直接exit，还要释放资源
    }

    log_init();
    if(ngx_signals_init() < 0){ // 如果信号初始化失败
        exitCode = 1;
        goto exit_label;
    }

    moveEnviron();
    setTitle("nginx: master"); // 设置主进程标题

    ngx_master_process_cycle();

exit_label:
    freeSource();
    return exitCode;
}

static void freeSource(){
    // 释放设置标题时分配的内存
    freeEnviron();

    // 关闭日志文件
    if(ngx_log.log_fd != STDERR_FILENO && ngx_log.log_fd != -1){        
        close(ngx_log.log_fd);
        ngx_log.log_fd = -1;     
    }
}