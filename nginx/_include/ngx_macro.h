#ifndef NGX_MACRO_H_
#define NGX_MACRO_H_

#include <string.h>

/**
 * 配置文件
 */
#define CONF_LINE_SIZE 500
#define CONF_NAME_SIZE 50
#define CONF_CONTENT_SIZE 400

/**
 * 日志
 */
typedef unsigned char u_char;

#define NGX_MAX_ERROR_STR   2048   //显示的错误信息最大数组长度

#define ngx_memcpy(dst, src, n) (((u_char *) memcpy(dst, src, n)) + (n)) // 对memcpy重新包装，使其返回末尾的位置

// 日志一共分成八个等级, 数字越小级别越高
enum Log_level{
    NGX_LOG_STDERR, // [stderr] -- 控制台错误，错误直接打印在屏幕上，不再写入日志文件
    NGX_LOG_EMERG,  // [emerg] -- 紧急
    NGX_LOG_ALERT,  // [alert] -- 警戒
    NGX_LOG_CRIT,   // [crit] -- 严重
    NGX_LOG_ERR,    // [error] -- 错误
    NGX_LOG_WARN,   // [warn] -- 警告
    NGX_LOG_NOTICE, // [notice] -- 注意
    NGX_LOG_INFO,   // [info] -- 信息
    NGX_LOG_DEBUG   // [debug] -- 调试
};

#define NGX_LOG_PATH "logs/error.log"

/**
 * 数字相关
 */
#define NGX_INT64_LEN 20 //64位有符号整型数据换成字符串形式的长度

/**
 * 进程
 */
enum Process_type{
    NGX_MASTER_PROCESS,
    NGX_WORKER_PROCESS
};

#endif