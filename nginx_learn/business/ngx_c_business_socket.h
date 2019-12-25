#ifndef NGX_C_BUSINESS_SOCKET_H_
#define NGX_C_BUSINESS_SOCKET_H_

#include "net/ngx_c_socket.h"

#pragma pack (1) // 1字节对齐

/**
 * 这两个结构用于测试服务器功能
*/
struct RegisterInfo{
	int           iType;          //类型
	char          username[56];   //用户名 
	char          password[40];   //密码

};

struct LoginInfo{
	char          username[56];   //用户名 
	char          password[40];   //密码

};

#pragma pack()

/**
 * @brief 继承自Socket的子类
*/
class BusinessSocket : public Socket{
public:
    BusinessSocket();
    virtual ~BusinessSocket();

    /**
     * @brief Socket对象在master进程中的初始化
    */
    virtual int ngx_socket_master_init();

    /**
     * @brief Socket对象在worker进程中的初始化
    */
    virtual int ngx_socket_worker_init();

    /**
     * @brief Socket对象在master进程中释放
    */
    virtual void ngx_socket_master_destroy();

    /**
     * @brief Socket对象在master进程中释放
    */
    virtual void ngx_socket_worker_destroy();

    virtual void ngx_recvMsg_handle(uint8_t * msg);

    void ngx_bussiness_register(MsgHeader *, uint8_t *, uint16_t);

    void ngx_bussiness_login(MsgHeader *, uint8_t *, uint16_t);
};

#endif // NGX_C_BUSINESS_SOCKET_H_
