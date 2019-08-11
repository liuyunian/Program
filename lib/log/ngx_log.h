#ifndef NGX_LOG_H_
#define NGX_LOG_H_

#include <stdarg.h>

typedef unsigned char u_char;

#define NGX_MAX_ERROR_SZ   2048   // 显示的错误信息最大长度

#define ngx_memcpy(dst, src, n) (((u_char *) memcpy(dst, src, n)) + (n)) // 对memcpy重新包装，使其返回末尾的位置

// 日志一共分成五个等级，数字越小级别越高
enum Log_level{
    NGX_LOG_FATAL,  // [fatal] -- 致命错误
    NGX_LOG_ERR,    // [error] -- 错误
    NGX_LOG_WARN,   // [warn]  -- 警告
    NGX_LOG_INFO,   // [info]  -- 提示
    NGX_LOG_DEBUG   // [debug] -- 调试
};

// [default] 日志文件输出路径
#define NGX_LOG_PATH "error.log"

// [default] 日志级别
#define NGX_LOG_LEVEL NGX_LOG_INFO

// [default] 默认在控制台中打印日志信息
#define NGX_LOG_OUTPUT 1

// 64位有符号整型数据换成字符串形式的长度
#define NGX_INT64_LEN 20

/**
 * @brief 日志设置信息
 */
struct LogInfor{
	int log_fd;     // log文件的描述符
	int log_level;  // log级别
    int log_output;    // 日志打印位置：0 仅打印到指定的日志文件 1 同时打印到控制台和日志文件
};

void ngx_log_init();
void ngx_log(int level, int err, const char * fmt, ...);
void ngx_log_close();

#endif // NGX_LOG_H_