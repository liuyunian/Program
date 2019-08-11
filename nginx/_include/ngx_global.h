#ifndef NGX_GLOBAL_H_
#define NGX_GLOBAL_H_

#include "ngx_log.h"

/**
 * 设置标题
 */
extern char ** g_argv;

/**
 * 日志打印
 */
extern struct LogInfor g_logInfor;

/**
 * 进程
 */
extern int g_procType; // 用于记录当前是master还是worker

#endif