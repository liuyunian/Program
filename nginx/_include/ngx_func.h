#ifndef NGX_FUNC_H__
#define NGX_FUNC_H__

#include <stdarg.h> // va_list
#include "ngx_macro.h" // u_char

/**
 * 字符串
 * ngx_string.cpp
*/
void Rtrim(char *string);
void Ltrim(char *string);

/**
 * 设置标题
 * ngx_setTitle.cpp
 */
void moveEnviron();
void setTitle(const char * title);
void freeEnviron();

/**
 * 日志
 * ngx_log.cpp
 */
void log_init();
void log(int level, int err, const char *fmt, ...);
void log_stderr(int level, int err, const char * fmt, ...);

/**
 * 打印
 * ngx_printf.cpp
 */
u_char * ngx_slprintf(u_char * buf, u_char * last, const char * fmt, ...);
u_char * ngx_vslprintf(u_char *buf, u_char *last, const char *fmt, va_list ap);

/**
 * 信号
 * ngx_signal.cpp
 */
int ngx_signals_init();

/**
 * 子进程
 * ngx_process_cycle.cpp
 */
void ngx_master_process_cycle();

/**
 * 守护进程
 */
int ngx_create_daemon();

#endif  