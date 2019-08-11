#ifndef NGX_FUNC_H_
#define NGX_FUNC_H_

/**
 * 设置标题
 * ngx_setTitle.cpp
 */
void moveEnviron();
void setTitle(const char * title);
void freeEnviron();

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
 * ngx_daemon.cpp
 */
int ngx_create_daemon();

/**
 * 通用网络函数
 * ngx_net_func.cpp
 */
int ngx_set_nonblocking(int sockfd);

#endif // NGX_FUNC_H_