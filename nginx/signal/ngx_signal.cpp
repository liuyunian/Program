#include <signal.h> // 信号相关
#include <string.h> // memset
#include <errno.h> // errno
#include <sys/wait.h> // waitpid

#include "ngx_func.h"
#include "ngx_macro.h"
#include "ngx_global.h"

static void ngx_signal_handler(int signo, siginfo_t *siginfo, void *ucontext);

static void ngx_process_get_status();

struct Signal{
    int signo; // 信号值
    const char * signame; // 信号名
    void (*handler)(int signo, siginfo_t * siginfo, void * ucontext); // 信号处理函数
};

Signal signals[] = {
    {SIGHUP, "SIGHUP", ngx_signal_handler}, // 标识1 -- 终端断开信号，对于守护进程常用于reload重载配置文件通知
    // {SIGINT, "SIGINT", ngx_signal_handler}, // 标识2
    {SIGQUIT, "SIGQUIT", ngx_signal_handler}, // 标识3
	{SIGTERM, "SIGTERM", ngx_signal_handler}, // 标识15
    {SIGCHLD, "SIGCHLD", ngx_signal_handler}, // 标识17 -- 子进程退出时，父进程会收到这个信号
    {SIGIO, "SIGIO", ngx_signal_handler}, // 标识29 -- 通用异步I/O信号
    {SIGSYS, "SIGSYS, SIG_IGN", NULL}, // 标识31 -- SIGSYS表示收到了一个无效系统调用，如果我们不忽略，进程会被操作系统杀死
    //...日后根据需要再继续增加
    {0, NULL, NULL} // 信号对应的数字至少是1，所以可以用0作为一个特殊标记
};

/**
 * @brief 信号初始化函数，用于给信号注册信号处理函数
 * @return 0：初始化成功 -1：初始化失败
 */
int ngx_signals_init(){
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
            ngx_log(NGX_LOG_ERR, errno, "sigaction(%s) failed", sig->signame);
            return -1;
        }
    }

    return 0;
}

static void 
ngx_signal_handler(int signo, siginfo_t * siginfo, void * ucontext){
    Signal * sig;
    for(sig = signals; sig->signo != 0; ++ sig){ // 这个for循环有啥用啊？？？
        if(sig->signo == signo){
            break;
        }
    }

    char * action = (char *)""; // 记录信号处理动作

    // if(g_procType == NGX_MASTER_PROCESS){
    //     switch(signo){
    //         case SIGCHLD:
    //             break;
    //         default:
    //             break;
    //     }
    // }
    // else if(g_procType == NGX_WORKER_PROCESS){
    //     // worker进程的信号处理
    // }
    // else{

    // }

    if(siginfo && siginfo->si_pid){
        ngx_log(NGX_LOG_INFO, 0, "signal %d (%s) received from %d%s", signo, sig->signame, siginfo->si_pid, action); 
    }
    else{
        ngx_log(NGX_LOG_INFO,0,"signal %d (%s) received %s", signo, sig->signame, action);//没有发送该信号的进程id，所以不显示发送该信号的进程id
    }

    if(signo == SIGCHLD){
        ngx_process_get_status();
    }
}

static void 
ngx_process_get_status(){
    pid_t pid;
    int status;
    int err;
    int one = 0; 

    for(;;){
        pid = waitpid(-1, &status, WNOHANG);
        if(pid == 0){ // 子进程还没结束
            return;
        }
        else if(pid == -1){
            err = errno;
            if(err == EINTR){ // 调用被某个信号中断
                continue;
            }

            if(err == ECHILD){ // 没有子进程
                if(!one){
                    ngx_log(NGX_LOG_INFO, err, "ngx_process_get_status函数中执行waitpid()失败");
                }

                return;
            }

            ngx_log(NGX_LOG_ERR, err, "ngx_process_get_status函数中执行waitpid()失败");

            return;
        }

        one = 1;
        if(WTERMSIG(status)){
            ngx_log(NGX_LOG_ERR, 0, "pid = %d exited on signal %d!", pid, WTERMSIG(status)); //获取使子进程终止的信号编号
        }
        else{
            ngx_log(NGX_LOG_INFO, 0, "pid = %d exited with code %d!", pid, WEXITSTATUS(status)); //WEXITSTATUS()获取子进程传递给exit或者_exit参数的低八位
        }
    }
}