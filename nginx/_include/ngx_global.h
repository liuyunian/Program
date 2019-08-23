#ifndef NGX_GLOBAL_H_
#define NGX_GLOBAL_H_

#include "app/ngx_log.h" // LogInfor
#include "business/ngx_c_business_socket.h" // BusinessSocket

/**
 * @brief 命令行参数
 * 设置标题时需要用到
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

/**
 * 网络
*/
extern BusinessSocket * g_sock;

#endif