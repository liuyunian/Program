#include <signal.h> // 信号相关
#include <errno.h> // errno
#include <unistd.h> // fork

#include "app/ngx_log.h"
#include "app/ngx_c_conf.h"
#include "_include/ngx_macro.h"
#include "_include/ngx_func.h"
#include "_include/ngx_global.h"
#include "net/ngx_c_threadPool.h"
#include "business/ngx_c_business_socket.h"

static void ngx_worker_process_cycle(int seq);

void ngx_master_process_cycle(){
    // [1] 屏蔽信号，不希望在fork子进程时被信号打断
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

    if(sigprocmask(SIG_BLOCK, &set, NULL) < 0){
        ngx_log(NGX_LOG_ERR, errno, "ngx_master_process_cycle()中sigprocmask()失败!");
    }

    // [2] fork子进程
    ConfFileProcessor * confProcessor = ConfFileProcessor::getInstance();
    int wpNum = confProcessor->ngx_conf_getContent_int("WorkProcess", NGX_WORKER_PROCESSES);
    for(int i = 0; i < wpNum; ++ i){
        pid_t pid = fork();
        if(pid == -1){ // 创建失败
            ngx_log(NGX_LOG_FATAL, errno, "ngx_worker_process_create()中fork()创建子进程失败，num = %d", i);
        }
        else if(pid == 0){ // 子进程
            ngx_worker_process_cycle(i);
        }
        else{ // 父进程
            continue;
        }
    }

    // [3] 清空信号集，不再屏蔽信号
    sigemptyset(&set);

    // [4] 反初始化并释放给Socket对象
    /**
     * 之前是这样考虑的：master进程是管理进程并不需要Socket对象负责通信
     * 所以这里考虑反初始化Socket对象 -- 关闭监听套接字，之后delete g_sock;
     * 
     * 但是如果考虑到work子进程终止之后master进程要负责再fork，那么此时就不能反初始化Socket对象
     * 因为这样的话子进程就没法创建监听套接字了
    */
    // g_sock->ngx_socket_master_destroy();
    // delete g_sock;

    // [5] 进入工作循环
    for(;;){
        /**
         * 阻塞在这里，等待一个信号，此时进程是挂起的，不占用cpu时间，只有收到信号才会被唤醒
         * 此时master进程完全靠信号驱动干活
        */
        sigsuspend(&set);
    }
}

static void ngx_worker_process_cycle(int seq){
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

        exit(1); // 直接退出，交由操作系统释放资源
    }

    // [3] 创建线程池
    ThreadPool * tp = ThreadPool::getInstance();
    ret = tp->ngx_threadPool_create();
    if(ret < 0){
        exit(1); // 直接退出，交由操作系统释放资源
    }

    // [4] 初始化Socket对象
    ret = g_sock->ngx_socket_worker_init();
    if(ret < 0){
        exit(1); // 直接退出，交由操作系统释放资源
    }

    // [5] ... 其他初始化

    ngx_log(NGX_LOG_INFO, 0, "nginx: worker %d 启动并开始运行......!", getpid());

    for(;;){
       g_sock->ngx_epoll_getEvent(-1);
    }

    // Socket对象反初始化
    g_sock->ngx_socket_worker_destroy();
    delete g_sock;

    // 线程池停止工作
    tp->ngx_threadPool_stop();

    // 释放设置标题时分配的内存
    ngx_free_environ();

    // 关闭日志文件
    ngx_log_close();
}