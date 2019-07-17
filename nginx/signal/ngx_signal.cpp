#include <signal.h>
#include <string.h>
#include <errno.h>

#include "ngx_func.h"
#include "ngx_macro.h"

static void signal_handler(int signo, siginfo_t *siginfo, void *ucontext);

struct Signal{
    int signo; // 信号值
    const char * signame; // 信号名
    void (*handler)(int signo, siginfo_t * siginfo, void * ucontext); // 信号处理函数
};

Signal signals[] = {
    {SIGHUP, "SIGHUP", signal_handler}, // 标识1 -- 终端断开信号，对于守护进程常用于reload重载配置文件通知
    // {SIGINT, "SIGINT", signal_handler}, // 标识2
    {SIGQUIT, "SIGQUIT", signal_handler}, // 标识3
	{SIGTERM, "SIGTERM", signal_handler}, // 标识15
    {SIGCHLD, "SIGCHLD", signal_handler}, // 标识17 -- 子进程退出时，父进程会收到这个信号
    {SIGIO, "SIGIO", signal_handler}, // 标识29 -- 通用异步I/O信号
    {SIGSYS, "SIGSYS, SIG_IGN", NULL}, // 标识31 -- SIGSYS表示收到了一个无效系统调用，如果我们不忽略，进程会被操作系统杀死
    //...日后根据需要再继续增加
    {0, NULL, NULL} // 信号对应的数字至少是1，所以可以用0作为一个特殊标记
};

/**
 * @brief 信号初始化函数，用于给信号注册信号处理函数
 * @return 0：初始化成功 -1：初始化失败
 */
int signals_init(){
    struct sigaction sa; // sigaction：系统定义的跟信号有关的一个结构，我们后续调用系统的sigaction()函数时要用到这个同名的结构

    Signal * sig;
    for(sig = signals; sig->signo != 0; ++ sig){
        memset(&sa, 0, sizeof(struct sigaction));

        if(sig->handler){ // 信号处理函数不为NULL
            sa.sa_sigaction = sig->handler;  // sa_sigaction：指定信号处理函数，注意sa_sigaction也是函数指针，是这个系统定义的结构sigaction中的一个成员（函数指针成员）
            sa.sa_flags = SA_SIGINFO; // sa_flags：int型，指定信号的一些选项，设置了该标记(SA_SIGINFO)，就表示信号附带的参数可以被传递到信号处理函数中
        }
        else{ // 信号处理函数为NULL，标识要忽略该信号
            sa.sa_handler = SIG_IGN; // sa_handler：和sa_sigaction一样，都是一个函数指针用来表示信号处理函数，只不过这两个函数指针他们参数不一样
                                    // sa_sigaction带的参数多，信息量大，而sa_handler带的参数少，信息量少；
                                    // 如果这里想用sa_sigaction，那么就需要把sa_flags设置为SA_SIGINFO
        }

        sigemptyset(&sa.sa_mask); // sa_mask是个信号集，用于表示要阻塞的信号

        if (sigaction(sig->signo, &sa, NULL) == -1){ // 使用sigaction函数注册信号处理函数
            log(NGX_LOG_EMERG, errno, "sigaction(%s) failed", sig->signame);
            return -1;
        }
    }

    return 0;
}

static void signal_handler(int signo, siginfo_t * siginfo, void * ucontext){
    log_stderr(NGX_LOG_INFO, 0, "Receive a signal %d", signo);
}