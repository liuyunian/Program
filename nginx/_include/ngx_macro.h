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
#define NGX_LOG_STDERR 0    //控制台错误【stderr】最高级别日志，日志的内容不再写入log参数指定的文件，而是会直接将日志输出到标准错误设备比如控制台屏幕
#define NGX_LOG_EMERG 1    //紧急 【emerg】
#define NGX_LOG_ALERT 2    //警戒 【alert】
#define NGX_LOG_CRIT 3    //严重 【crit】
#define NGX_LOG_ERR 4    //错误 【error】
#define NGX_LOG_WARN 5    //警告 【warn】
#define NGX_LOG_NOTICE 6    //注意 【notice】
#define NGX_LOG_INFO 7    //信息 【info】
#define NGX_LOG_DEBUG 8    //调试 【debug】

#define NGX_LOG_PATH "logs/error.log"
/**
 * 数字相关
 */
#define NGX_INT64_LEN 20 //64位有符号整型数据换成字符串形式的长度

#endif