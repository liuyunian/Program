#include <signal.h> // 信号相关
#include <errno.h> // errno
#include <unistd.h> // fork

#include "ngx_log.h"
#include "ngx_macro.h"
#include "ngx_func.h"
#include "ngx_global.h"
#include "ngx_c_conf.h"

static void ngx_start_worker_processes(int wp_num);

static void ngx_worker_process_cycle(int seq);

static void ngx_worker_process_init(int seq);

void ngx_master_process_cycle(){
    sigset_t set;

    sigemptyset(&set);

    sigaddset(&set, SIGCHLD);     //子进程状态改变
    sigaddset(&set, SIGALRM);     //定时器超时
    sigaddset(&set, SIGIO);       //异步I/O
    sigaddset(&set, SIGINT);      //终端中断符
    sigaddset(&set, SIGHUP);      //连接断开
    sigaddset(&set, SIGUSR1);     //用户定义信号
    sigaddset(&set, SIGUSR2);     //用户定义信号
    sigaddset(&set, SIGWINCH);    //终端窗口大小改变
    sigaddset(&set, SIGTERM);     //终止
    sigaddset(&set, SIGQUIT);     //终端退出符

    if(sigprocmask(SIG_BLOCK, &set, NULL) < 0){ // 屏蔽上述信号，不希望在fork子进程时被信号打断
        ngx_log(NGX_LOG_ERR, errno, "ngx_master_process_cycle()中sigprocmask()失败!");
    }

    ConfFileProcessor * confProcessor = ConfFileProcessor::getInstance();
    int wp_num = confProcessor->getItemContent_int("WorkProcess", NGX_WORKER_PROCESSES);
    ngx_start_worker_processes(wp_num);

    sigemptyset(&set); // 清空信号集

    for(;;){ // master process进入工作循环
        // sleep(1);

        sigsuspend(&set); // 阻塞在这里，等待一个信号，此时进程是挂起的，不占用cpu时间，只有收到信号才会被唤醒，此时master进程完全靠信号驱动干活
    }
}

static void 
ngx_start_worker_processes(int wp_num){
    for(int i = 0; i < wp_num; ++ i){
        pid_t pid = fork();
        if(pid == -1){ // 创建失败
            ngx_log(NGX_LOG_FATAL, errno, "ngx_start_worker_processes()中fork()创建子进程失败，num = %d", i);
        }
        else if(pid == 0){
            ngx_worker_process_cycle(i);
        }
        else{
            continue;
        }
    }

    // master进程会执行到这里
    // 如有需要，再增加代码
}

static void
ngx_worker_process_cycle(int seq){
    ngx_worker_process_init(seq);

    g_procType = NGX_WORKER_PROCESS;
    setTitle("nginx: worker");
    ngx_log(NGX_LOG_INFO, 0, "nginx: worker %d 启动并开始运行......!", getpid());

    for(;;){ // worker process进入工作循环
        // sleep(1);
    }
}

static void 
ngx_worker_process_init(int seqs){
    sigset_t set;
    sigemptyset(&set);
    if(sigprocmask(SIG_SETMASK, &set, NULL) < 0){
        ngx_log(NGX_LOG_ERR, errno, "ngx_worker_process_init()中sigprocmask()失败!");
    }

    // 之后增加代码
}