#include <stdarg.h> // va_list
#include <string.h> // memset
#include <stdio.h>  // sprintf
#include <stdint.h> // u_int64_t...
#include <unistd.h> // wirte, STDERR_FILENO
#include <fcntl.h>  // open
#include <errno.h>  // errno
#include <time.h>   // localtime_r
#include <math.h>   // pow
#include <sys/time.h> //gettimeofday

#include "ngx_log.h"

static struct LogInfor g_logInfor;

static u_char err_levels[][20] = {
    {"FATAL"},
    {"ERROR"},
    {"WARN"},
    {"INFO"},
    {"DEBUG"}
};

static u_char * ngx_log_errno(u_char * buf, u_char * last, int err);

static u_char * ngx_log_slprintf(u_char * buf, u_char * last, const char * fmt, ...);

static u_char * ngx_log_vslprintf(u_char * buf, u_char * last, const char * fmt, va_list ap);

static u_char * ngx_log_sprintfNum(u_char *, u_char *, uint64_t, u_char, int, uint32_t);

void ngx_log_init(){
    /*
        一般情况下结合配置文件设置log系统的参数
        比如：日志文件路径、默认的日志级别、日志是否在控制台中打印
        配置文件中没有相关信息的话再从宏定义中获取相关的信息
    */
    // ConfFileProcessor * confProcessor = ConfFileProcessor::getInstance();
    // const char * fileName = confProcessor->getItemContent_str("LogFile");
    // if(fileName == NULL){ // 配置文件中没有设置LogFile的值
    //     fileName = NGX_LOG_PATH;
    // }

    // g_logInfor.log_level = confProcessor->getItemContent_int("LogLevel", NGX_LOG_LEVEL);

    // if(confProcessor->getItemContent_int("Daemon", NGX_IS_DAEMON) == 1){
    //     g_logInfor.log_out = 0;
    // }
    
    g_logInfor.log_level = NGX_LOG_LEVEL;
    g_logInfor.log_output = NGX_LOG_OUTPUT;
    const char * fileName = NGX_LOG_PATH;

    g_logInfor.log_fd = open(fileName, O_WRONLY|O_APPEND|O_CREAT, 0644);
    if (g_logInfor.log_fd < 0){
        printf("log_init()函数中打开日志文件失败，%s\n", strerror(errno));
        g_logInfor.log_fd = STDERR_FILENO; // 直接定位到标准错误去了, 这就是直接打印到屏幕上  
    } 
}

void ngx_log_close(){
    if(g_logInfor.log_fd != STDERR_FILENO && g_logInfor.log_fd != -1){        
        int ret = close(g_logInfor.log_fd);
        if(ret < 0){
            printf("ngx_log_close()函数中调用close()函数出错，%s", strerror(errno));
        }
        
        g_logInfor.log_fd = -1;     
    }

}

void ngx_log(int level, int err, const char *fmt, ...){
    va_list ap;

    u_char errStr[NGX_MAX_ERROR_SZ];
    memset(errStr, 0, NGX_MAX_ERROR_SZ);

    u_char * last, * index;
    last = errStr + NGX_MAX_ERROR_SZ; // last指向数组最后一个有效字节的后面

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
    ngx_log_slprintf(curTime, last, "%4d/%02d/%02d %02d:%02d:%02d", //格式是: yyyy/mm/dd hh:mm:ss
                    tm.tm_year, tm.tm_mon,
                    tm.tm_mday, tm.tm_hour,
                    tm.tm_min, tm.tm_sec
                );

    index = ngx_memcpy(errStr, curTime, strlen((const char *)curTime)); // 时间
    index = ngx_log_slprintf(index, last, " [%s] ", err_levels[level]); // 日志等级
    index = ngx_log_slprintf(index, last, "%d: ", getpid()); // 进程ID

    va_start(ap, fmt);
    index = ngx_log_vslprintf(index, last, fmt, ap); // 格式化字符串
    va_end(ap);

    if(err){
        index = ngx_log_errno(index, last, err);
    }

    if (index >= (last -1)){
        index = (last - 1) - 1;
    }
    *index = '\n'; //末尾添加换行符 
    ++ index; 

    if(level <= g_logInfor.log_level){
        ssize_t len = write(g_logInfor.log_fd, errStr, index - errStr);
        if(len < 0){
            printf("ngx_log()中向日志文件写日志信息失败，%s\n", strerror(errno));
        }

        if(g_logInfor.log_output){
            write(STDERR_FILENO, errStr, index - errStr);
        }
    }
}

u_char * ngx_log_errno(u_char * buf, u_char * last, int err){
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

u_char * ngx_log_slprintf(u_char * buf, u_char * last, const char * fmt, ...){
    va_list ap;

    va_start(ap, fmt);
    buf = ngx_log_vslprintf(buf, last, fmt, ap);
    va_end(ap);

    return buf;
}

u_char * ngx_log_vslprintf(u_char * buf, u_char * last, const char * fmt, va_list ap){
    int64_t i64;    // 保存有符号整数
    uint64_t ui64;  // 保存无符号整数
    char ch;        // 保存字符
    char * string;  // 保存字符串
    double fn;      // 保存浮点数
    u_int64_t frac; // 保存小数部分

    u_char padding;         // 记录填充的字符，空格" " or 空字符
    int hex;                // 是否以16进制形式显示，0：不是，1：是，并以小写字母显示a-f，2：是，并以大写字母显示A-F
    u_int32_t width;        // 用于存放%5d, %7f这种%后面的数字
    u_int32_t frac_width;   // 用于保存%7.2这种小数点后的位数

    while(*fmt && buf < last){
        if(*fmt == '%'){
            // 初始化
            i64 = 0;
            ui64 = 0;
            ch = 0;
            string = NULL;
            fn = 0;
            frac = 0;

            hex = 0;
            width = 0;
            frac_width = 6; // 默认是显示小数点后6位，如果实际的数字不够，则填充0

            ++ fmt;
            padding = (*fmt == '0' ? '0' : ' ');

            while(*fmt >= '0' && *fmt <= '9'){
                width = width * 10 + (*fmt - '0');
                ++ fmt;
            }

            while(1){
                switch(*fmt){
                    case 'x': {
                        hex = 1;
                        ++ fmt;
                        continue;
                    }

                    case 'X': {
                        hex = 2;
                        ++ fmt;
                        continue;
                    }

                    case '.': {
                        ++ fmt;
                        frac_width = 0;
                        while(*fmt >= 0 && *fmt <= '9'){
                            frac_width = frac_width * 10 + (*fmt - '0');
                            ++ fmt;
                        }
                        continue;
                    }
                    
                    default: {
                        break; // leave switch
                    }
                }

                break; // leave while
            }

            switch(*fmt){
                case '%': {
                    *buf = '%';
                    ++ buf;
                    ++ fmt;
                    continue; //针对while循环，对switch没有作用
                }

                case 'd': {
                    i64 = (int64_t)va_arg(ap, int);
                    if(i64 < 0){
                        *buf = '-';
                        ++ buf;

                        ui64 = (u_int64_t)(-i64);
                    }
                    else{
                        ui64 = (u_int64_t)i64;
                    }
                    buf = ngx_log_sprintfNum(buf, last, ui64, padding, hex, width);

                    ++ fmt;
                    continue;
                }

                case 'u': {
                    ui64 = va_arg(ap, u_int64_t);
                    buf = ngx_log_sprintfNum(buf, last, ui64, padding, hex, width);

                    ++ fmt;
                    continue;
                }

                case 'c': {
                    ch = (char)va_arg(ap, int);
                    *buf = ch;
                    ++ buf;

                    ++ fmt;
                    continue;
                }

                case 's':{
                    string = va_arg(ap, char *);
                    size_t string_len = strlen(string);
                    if(buf + string_len >= last){
                        string_len = last - buf;
                    }
                    buf = ngx_memcpy(buf, string, string_len);

                    ++ fmt;
                    continue;
                }

                case 'f': {
                    fn = va_arg(ap, double);
                    if(fn < 0){
                        *buf = '-';
                        ++ buf;
                        fn = -fn;
                    }
                    ui64 = (u_int64_t)fn; // 保存浮点数的整数部分
                    frac = (fn - ui64) * pow(10, frac_width) + 0.5; // 保留浮点数的小数部分
                    if(frac == pow(10, frac_width)){
                        ++ ui64;
                        frac = 0;
                    }

                    buf = ngx_log_sprintfNum(buf, last, ui64, padding, 0, width); // 将整数部分放入buf
                    if(buf < last){
                        *buf = '.';
                        ++ buf;
                    }
                    buf = ngx_log_sprintfNum(buf, last, frac, '0', 0, frac_width); // 将小数部分放入buf

                    ++ fmt;
                    continue;
                }
                /*
                    后面逐步增加的格式
                    case 'e' 以指数形式输出单、双精度实数
                    case 'g' 以%f%e中较短的输出宽度输出单、双精度实数
                    case 'o' 以八进制形式输出无符号整数(不输出前缀O) 
                    case 'p' 输出地址
                 */

                default: {
                    *buf = *fmt;
                    ++ buf;
                    ++ fmt;
                }
            }
        }
        else{
            *buf = *fmt;
            ++ buf;
            ++ fmt;
        }
    }

    return buf;
}

static u_char * ngx_log_sprintfNum(u_char * buf, u_char * last, uint64_t ui64, u_char padding, int hexFlag, uint32_t width){
    u_char temp[NGX_INT64_LEN + 1]; // temp[0...20]
    memset(temp, 0, NGX_INT64_LEN + 1); // 置零
    u_char * tmp_last = temp + NGX_INT64_LEN; // NGX_INT64_LEN = 20，所以p指向的是temp[20]那个位置，也就是数组最后一个元素位置，这个位置必须是'\0' 

    static u_char hex[] = "0123456789abcdef";
    static u_char HEX[] = "0123456789ABCDEF";

    if(hexFlag == 0){ // 不以16进制显示
        do {
            -- tmp_last;
            *tmp_last = (u_char) (ui64 % 10 + '0');
        } while (ui64 /= 10); // 从后往前添加数据
    }
    else if(hexFlag == 1){ // 以小写字母的形式显示16进制整数，比如30，显示为1e
        do {            
            -- tmp_last;
            *tmp_last = hex[(uint32_t) (ui64 & 0xf)]; 
        } while (ui64 >>= 4);
    } 
    else{ // 以大写字母的形式显示16进制整数，比如30，显示为1E
        do { 
            -- tmp_last;
            *tmp_last = HEX[(uint32_t) (ui64 & 0xf)]; 
        } while (ui64 >>= 4);
    }

    u_int32_t len = (temp + NGX_INT64_LEN) - tmp_last; // temp填有数据的长度，比如123，len = 3
    while (len < width){ //如果希望显示的宽度是%12d，而实际的数是7654321，只有7个宽度，那么这里要填充5个0
        -- tmp_last;
        *tmp_last = padding;
        ++ len;
    }

    if((buf + len) >= last){
        len = last - buf; //剩余的buf有多少字节就拷贝多少
    }

    return ngx_memcpy(buf, tmp_last, len);
}