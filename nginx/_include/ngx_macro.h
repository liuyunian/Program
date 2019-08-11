#ifndef NGX_MACRO_H_
#define NGX_MACRO_H_

#include <string.h>

/**
 * 配置文件
 */
#define CONF_LINE_SZ 500
#define CONF_NAME_SZ 50
#define CONF_CONTENT_SZ 400

/**
 * 进程
 */
enum Process_type{
    NGX_MASTER_PROCESS,
    NGX_WORKER_PROCESS
};

// [default]worker进程数目
#define NGX_WORKER_PROCESSES 1

// [default]是否采用守护进程方式运行
#define NGX_IS_DAEMON 0

/**
 * socket & epoll
 */
// [default]监听端口数目
#define NGX_PROT_COUNT 1

// [default]监听端口
#define NGX_LISTEN_PORT 80

#define NGX_LISTEN_BACKLOG 511

// [default]连接池大小
#define NGX_WORKER_CONNECTIONS 1024

// 一次从epoll对象中获取的最大事件数
#define NGX_MAX_EVENTS 512

/**
 * 收发包
 */
//数据包的最大长度
#define PKT_MAX_LEN  10240

// 收包状态
enum Recv_pkt_state{
    INVALID_STATE = -1,
    RECV_PKT_HEADER,
    RECV_PKT_BODY
};

#endif // NGX_MACRO_H_