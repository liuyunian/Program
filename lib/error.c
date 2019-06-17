#include <stdarg.h> // va_list va_start va_end
#include <syslog.h> // syslog
#include <errno.h> // errno
#include <stdio.h> //stdout stderr
#include <string.h> // strlen strcat
#include <stdlib.h> // exit abort

#define MAXLINE 4096

int daemon_proc;

static void err_doit(int, int, const char *, va_list);

/**
 * 涉及系统调用的非致命错误
 * 打印信息并返回
*/
void err_ret(const char * fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    err_doit(1, LOG_INFO, fmt, ap);
    va_end(ap);

    return;
}

/**
 * 涉及系统调用的致命错误
 * 打印信息并终止
 */
void err_sys(const char * fmt, ...){
    va_list ap;

    va_start(ap, fmt);
    err_doit(1, LOG_ERR, fmt, ap);
    va_end(ap);

    exit(1);
}

/**
 * 涉及系统调用的致命错误
 * 打印信息，核心转储，并终止
 */
void err_dump(const char * fmt, ...){
    va_list ap;

    va_start(ap, fmt);
    err_doit(1, LOG_ERR, fmt, ap);
    va_end(ap);

    abort();
    exit(1);
}

/**
 * 不涉及系统调用的非致命错误
 * 打印信息并返回
 */
void err_msg(const char * fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    err_doit(0, LOG_INFO, fmt, ap);
    va_end(ap);

    return;
}

/**
 * 不涉及系统调用的致命错误
 * 打印信息并终止
 */
void err_quit(const char * fmt, ...){
    va_list ap;

    va_start(ap, fmt);
    err_doit(0, LOG_ERR, fmt, ap);
    va_end(ap);

    exit(1);
}

/**
 * 打印信息并返回函数调用者
 * 函数调用者指定"errnoflag"和"level"
 */

static void 
err_doit(int errnoflag, int level, const char * fmt, va_list ap){
    int errno_save, buf_len;
    char buf[MAXLINE + 1];

    errno_save = errno;

#ifdef HAVE_VSNPRINTF
    vsnprintf(buf, MAXLINE, fmt, ap);
#else
    vsprintf(buf, fmt, ap);
#endif

    buf_len = strlen(buf);
    if(errnoflag){
        snprintf(buf + buf_len, MAXLINE - buf_len, ": %s", strerror(errno_save));
    }
    strcat(buf, "\n");

    if(daemon_proc){
        syslog(level, "%s", buf);
    }
    else{
        fflush(stdout);
        fputs(buf, stderr);
        fflush(stderr);
    }

    return;
}