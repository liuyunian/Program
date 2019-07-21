#include <stdarg.h> // va_list
#include <string.h> // memset
#include <stdio.h> // sprintf
#include <stdint.h> // u_int64_t...
#include <unistd.h> // wirte, STDERR_FILENO
#include <fcntl.h> // open
#include <errno.h> // errno
#include <sys/time.h> //gettimeofday
#include <time.h> //localtime_r

#include "ngx_macro.h"
#include "ngx_func.h"
#include "ngx_c_conf.h"

extern int errno;

static u_char * log_errno(u_char * buf, u_char * last, int err);

static u_char err_levels[][20] = {
    {"stderr"},    //0：控制台错误
    {"emerg"},     //1：紧急
    {"alert"},     //2：警戒
    {"crit"},      //3：严重
    {"error"},     //4：错误
    {"warn"},      //5：警告
    {"notice"},    //6：注意
    {"info"},      //7：信息
    {"debug"}      //8：调试
};

void log_stderr(int level, int err, const char * fmt, ...){
    va_list ap;

    u_char errStr[NGX_MAX_ERROR_STR];
    memset(errStr, 0, NGX_MAX_ERROR_STR);

    u_char * last, * index;
    last = errStr + NGX_MAX_ERROR_STR; // last指向数组最后一个有效字节的后面
    index = ngx_slprintf(errStr, last, "[%s] ", err_levels[level]); // 日志等级
    index = ngx_slprintf(index, last, "%d: ", getpid()); // 进程ID

    va_start(ap, fmt);
    index = ngx_vslprintf(index, last, fmt, ap);
    va_end(ap);

    if(err){
        index = log_errno(index, last, err);
    }

    if (index >= (last -1)){
        index = (last - 1) - 1;
    }
    *index = '\n'; //末尾添加换行符 
    ++ index; 
 
    write(STDERR_FILENO, errStr, index - errStr);
}

u_char * log_errno(u_char * buf, u_char * last, int err){
    char * errnoStr = strerror(err);
    size_t errnoLen = strlen(errnoStr);

    char leftStr[10] = {0};
    sprintf(leftStr, " (%d: ", err);
    size_t leftStrLen = strlen(leftStr);

    char rightStr[] = ")";
    size_t rightStrLen = strlen(rightStr);

    if(buf + leftStrLen + errnoLen + rightStrLen < last){
        buf = ngx_memcpy(buf, leftStr, leftStrLen);
        buf = ngx_memcpy(buf, errnoStr, errnoLen);
        buf = ngx_memcpy(buf, rightStr, rightStrLen);
    }

    return buf;
}

void log_init(){
    ConfFileProcessor * confProcessor = ConfFileProcessor::getInstance();
    const char * fileName = confProcessor->getItemContent_str("LogFile");
    if(fileName == NULL){ // 配置文件中没有设置LogFile的值
        fileName = NGX_LOG_PATH;
    }
    g_logInfor.log_level = confProcessor->getItemContent_int("LogLevel", 6);

    g_logInfor.log_fd = open(fileName, O_WRONLY|O_APPEND|O_CREAT, 0644);
    if (g_logInfor.log_fd < 0){
        log_stderr(NGX_LOG_ERR, errno, "Failed to open error log file");
        g_logInfor.log_fd = STDERR_FILENO; //直接定位到标准错误去了, 这就是直接打印到屏幕上  
    } 
}

void log(int level, int err, const char *fmt, ...){
    va_list ap;

    u_char errStr[NGX_MAX_ERROR_STR];
    memset(errStr, 0, NGX_MAX_ERROR_STR);

    u_char * last, * index;
    last = errStr + NGX_MAX_ERROR_STR; // last指向数组最后一个有效字节的后面

    struct timeval   tv;
    struct tm        tm;
    memset(&tv,0,sizeof(struct timeval));    
    memset(&tm,0,sizeof(struct tm));

    gettimeofday(&tv, NULL); // 获取当前时间，返回自1970-01-01 00:00:00到现在经历的秒数，第二个参数是时区，一般不关心
    time_t sec = tv.tv_sec; // 秒
    localtime_r(&sec, &tm); // 把参数1的time_t转换为本地时间，保存到参数2中去，带_r的是线程安全的版本，尽量使用
    ++ tm.tm_mon; // 月份要调整下正常
    tm.tm_year += 1900; // 年份要调整下才正常

    u_char curTime[40] = {0};
    ngx_slprintf(curTime, last, "%4d/%02d/%02d %02d:%02d:%02d", //格式是: yyyy/mm/dd hh:mm:ss
                    tm.tm_year, tm.tm_mon,
                    tm.tm_mday, tm.tm_hour,
                    tm.tm_min, tm.tm_sec
                );

    index = ngx_memcpy(errStr, curTime, strlen((const char *)curTime)); // 时间
    index = ngx_slprintf(index, last, " [%s] ", err_levels[level]); // 日志等级
    index = ngx_slprintf(index, last, "%d: ", getpid()); // 进程ID

    va_start(ap, fmt);
    index = ngx_vslprintf(index, last, fmt, ap); // 格式化字符串
    va_end(ap);

    if(err){
        index = log_errno(index, last, err);
    }

    if (index >= (last -1)){
        index = (last - 1) - 1;
    }
    *index = '\n'; //末尾添加换行符 
    ++ index; 

    if(level <= g_logInfor.log_level){
        ssize_t len = write(g_logInfor.log_fd, errStr, index - errStr);
        if(len < 0){
            write(STDERR_FILENO, errStr, index - errStr);
        }
    }
}