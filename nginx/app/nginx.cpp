#include <unistd.h>
#include <stdio.h>

#include "ngx_log.h"
#include "ngx_c_conf.h"
#include "ngx_func.h"
#include "ngx_macro.h"
#include "ngx_c_socket.h"
#include "ngx_global.h"

char ** g_argv;

int g_procType;

struct LogInfor g_logInfor;

Socket g_sock;

int main(int argc, char * argv[]){
    // 无关紧要的初始化
    g_argv = argv;
    g_procType = NGX_MASTER_PROCESS;

    // 加载配置文件
    ConfFileProcessor * confProcessor = ConfFileProcessor::getInstance();
    if(!confProcessor->ngx_conf_load("nginx.conf")){
        printf("Fail to load %s, exit\n", "nginx.conf");
        exit(1); // 不需要释放资源，直接退出，exit(0) == return 0;正常退出，exit(1)获知exit(-1)错误退出
    }

    // 初始化日志
    ngx_log_init();

    // 设置进程标题
    ngx_move_environ();
    ngx_set_title("nginx: master"); // 设置主进程标题

    // 初始化信号
    if(ngx_signals_init() < 0){
        exitCode = 1;
        goto exit_label;
    }

    // 是否以守护进程方式运行
    if(confProcessor->ngx_conf_getContent_int("Daemon", NGX_IS_DAEMON) == 1){
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


    ngx_log(NGX_LOG_INFO, 0, "nginx: master %d 启动并开始运行......!", getpid());

    // 进入master进程工作循环
    ngx_master_process_cycle();

exit_label:
    // 释放设置标题时分配的内存
    ngx_free_environ();

    // 关闭日志文件
    ngx_log_close();
    
    return exitCode;
}