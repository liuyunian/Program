#include <unistd.h>
#include <stdio.h>

#include "ngx_c_conf.h"
#include "ngx_func.h"
#include "ngx_macro.h"

static void freeSource();

char ** g_argv;
int g_procType;
LogInfor g_logInfor;

int main(int argc, char * argv[]){
    g_argv = argv;
    int exitCode = 0;

    // 加载配置文件
    ConfFileProcessor * confProcessor = ConfFileProcessor::getInstance();
    if(!confProcessor->load("nginx.conf")){
        log_stderr(NGX_LOG_ERR, 0, "Fail to load %s, exit\n", "nginx.conf");
        exitCode = 2;
        goto exit_label; // 不能直接exit，还要释放资源
    }

    // 初始化日志
    log_init();

    // 初始化信号
    if(ngx_signals_init() < 0){ // 如果信号初始化失败
        exitCode = 1;
        goto exit_label;
    }

    // 是否以守护进程方式运行
    if(confProcessor->getItemContent_int("Paemon", 0) == 1){
        int ret_daemon = ngx_create_daemon();
        if(ret_daemon < 0){
            exitCode = 1;
            goto exit_label;
        }
        else if(ret_daemon == 1){
            exitCode = 0;
            goto exit_label;
        }
    }

    // 设置父进程标题
    moveEnviron();
    g_procType = NGX_MASTER_PROCESS;
    setTitle("nginx: master"); // 设置主进程标题
    log(NGX_LOG_NOTICE, 0, "nginx: master %d 启动并开始运行......!", getpid());

    // 进入master进程工作循环
    ngx_master_process_cycle();

exit_label:
    freeSource();
    return exitCode;
}

static void freeSource(){
    // 释放设置标题时分配的内存
    freeEnviron();

    // 关闭日志文件
    if(g_logInfor.log_fd != STDERR_FILENO && g_logInfor.log_fd != -1){        
        close(g_logInfor.log_fd);
        g_logInfor.log_fd = -1;     
    }
}