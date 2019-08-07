#ifndef NGX_GLOBAL_H_
#define NGX_GLOBAL_H_

#include "ngx_macro.h"

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

extern LogInfor g_logInfor;

/**
 * 进程
 */
extern int g_procType; // 用于记录当前是master还是worker

#endif