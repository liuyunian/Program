#ifndef NGX_FUNC_H_
#define NGX_FUNC_H_

#include <stdint.h> // uint32_t

/**
 * 设置标题
 * ngx_setTitle.cpp
 */
void ngx_move_environ();
void ngx_set_title(const char * title);
void ngx_free_environ();

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

/**
 * CRC32校验
 * ndn_crc32.cpp
*/
void ngx_crc32_table_init();

int ngx_crc32_get_value(unsigned char * buffer, uint32_t dwSize);


#endif // NGX_FUNC_H_