#ifndef NGX_GLOBAL_H_
#define NGX_GLOBAL_H_

#include "ngx_macro.h"

/**
 * 配置文件
 */
struct ConfItem{
	char itemName[CONF_NAME_SIZE];
	char itemContent[CONF_CONTENT_SIZE];
};

/**
 * 设置标题
 */
extern char ** g_argv;

/**
 * 日志打印
 */
struct LogInfor{
	int log_fd; // log文件的描述符
	int log_level; // log级别
};

extern LogInfor ngx_log;

#endif