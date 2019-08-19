#include <signal.h> // 信号相关
#include <errno.h> // errno
#include <unistd.h> // fork

#include "ngx_log.h"
#include "ngx_macro.h"
#include "ngx_func.h"
#include "ngx_global.h"
#include "ngx_c_conf.h"
#include "ngx_c_threadPool.h"
#include "ngx_c_business_socket.h"

BusinessSocket g_sock;

static void ngx_worker_process_create(int wp_num);

static void ngx_worker_process_cycle(int seq);

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
    int wp_num = confProcessor->ngx_conf_getContent_int("WorkProcess", NGX_WORKER_PROCESSES);
    ngx_worker_process_create(wp_num);

    sigemptyset(&set); // 清空信号集

    for(;;){ // master process进入工作循环
        // sleep(1);

        sigsuspend(&set); // 阻塞在这里，等待一个信号，此时进程是挂起的，不占用cpu时间，只有收到信号才会被唤醒，此时master进程完全靠信号驱动干活
    }
}

static void 
ngx_worker_process_create(int wp_num){
    for(int i = 0; i < wp_num; ++ i){
        pid_t pid = fork();
        if(pid == -1){ // 创建失败
            ngx_log(NGX_LOG_FATAL, errno, "ngx_worker_process_create()中fork()创建子进程失败，num = %d", i);
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
    // [1] 无关紧要
    int ret = 0; // 记录返回值
    g_procType = NGX_WORKER_PROCESS;
    ngx_set_title("nginx: worker");

    // [2] 解除屏蔽的信号
    sigset_t set;
    sigemptyset(&set);
    ret = sigprocmask(SIG_SETMASK, &set, NULL);
    if(ret < 0){
        ngx_log(NGX_LOG_ERR, errno, "ngx_worker_process_init()中sigprocmask()失败!，num = %d", seq);

        // 释放设置标题时分配的内存
        ngx_free_environ();

        // 关闭日志文件
        ngx_log_close();

        exit(1);
    }

    // [3] 初始化监听套接字，开始接受TCP连接
    ret = g_sock.ngx_sockets_init();
    if(ret < 0){
        // 释放设置标题时分配的内存
        ngx_free_environ();

        // 关闭日志文件
        ngx_log_close();

        exit(1);
    }

    // [4] 创建线程池
    ThreadPool * tp = ThreadPool::getInstance();
    ret = tp->ngx_threadPool_create();
    if(ret < 0){
        // 关闭监听套接字
        g_sock.ngx_sockets_close();

        // 释放设置标题时分配的内存
        ngx_free_environ();

        // 关闭日志文件
        ngx_log_close();

        exit(1);
    }

    // [5] 初始化epoll
    ret = g_sock.ngx_epoll_init();
    if(ret < 0){
        // 关闭监听套接字
        g_sock.ngx_sockets_close();

        // 线程池停止工作
        tp->ngx_threadPool_stop();

        // 释放设置标题时分配的内存
        ngx_free_environ();

        // 关闭日志文件
        ngx_log_close();

        exit(1);
    }

    // ... 其他初始化

    ngx_log(NGX_LOG_INFO, 0, "nginx: worker %d 启动并开始运行......!", getpid());

    for(;;){ 
        // sleep(1);
       g_sock.ngx_epoll_getEvent(-1);
    }

    // 关闭监听套接字
    g_sock.ngx_sockets_close();

    // 线程池停止工作
    tp->ngx_threadPool_stop();

    // 释放设置标题时分配的内存
    ngx_free_environ();

    // 关闭日志文件
    ngx_log_close();
}