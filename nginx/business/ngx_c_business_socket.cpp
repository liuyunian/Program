
#include "ngx_log.h"
#include "ngx_func.h"
#include "ngx_macro.h"
#include "ngx_c_connectionPool.h"
#include "ngx_c_business_socket.h"

/**
 * 客户端发来的包中有类型字段，服务器端根据类型值确定该数据包如何处理
 * 这里实现类型与处理逻辑对应关系是通过下标和函数指针数组实现的，类型值作为数组下标从函数指针数组中找到对应的处理函数
 * 比起if语句判断，这种方式更加高效，代码可读性更高
 * 这种思路就要求将具体的处理函数统一格式，这样才可以放入数组
*/
typedef void (BusinessSocket:: * handler)(
    TCPConnection * c,      // TCP连接对象
    MsgHeader * msgHeader,  // 消息头
    uint8_t * pktBody,      // 包体
    uint16_t pktBodyLen    // 包体长度
);

static const handler typeHandlers[] = {
    nullptr,    // 前5个元素保留
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    &BusinessSocket::ngx_bussiness_register,
    &BusinessSocket::ngx_bussiness_login
};

BusinessSocket::BusinessSocket(){}

BusinessSocket::~BusinessSocket(){}

static const int maxValidType = sizeof(typeHandlers) / sizeof(handler); // 可以进行处理的最大有效类型

void BusinessSocket::ngx_msg_handle(uint8_t * msg){
    const int PKT_HEADER_SZ = sizeof(PktHeader);
    const int MSG_HEADER_SZ = sizeof(MsgHeader);

    MsgHeader * msgHeader = (MsgHeader *)msg;
    TCPConnection * c = msgHeader->c;

    // 判断该消息是否过期，消息在消息队列中提留时，如果该消息对应的连接断开，那么该消息就过期了，不进行处理
    if(c->curSeq != msgHeader->curSeq){
        return;
    }

    PktHeader * pktHeader = (PktHeader *)(msg + MSG_HEADER_SZ); // 偏移出消息头指向包头
    uint8_t * pktBody = nullptr;
    uint16_t pktLen = ntohs(pktHeader->len);
    uint16_t pktBodyLen = pktLen - PKT_HEADER_SZ;
    int crc32 = ntohl(pktHeader->crc32);

    // 校验CRC
    if(PKT_HEADER_SZ == pktLen){ // 没有包体
        if(crc32 != 0){
            return;
        }
    }
    else{
        pktBody = msg + MSG_HEADER_SZ + PKT_HEADER_SZ;
        if(crc32 != ngx_crc32_get_value(pktBody, pktBodyLen)){
            return;
        }
    }

    uint16_t type = ntohs(pktHeader->type);

    // 校验类型值是否有效
    if(type > maxValidType){
        return;
    }

    // 调用具体的业务逻辑处理函数
    if(typeHandlers[type] != nullptr){
        (this->*typeHandlers[type])(c, msgHeader, pktBody, pktBodyLen);
    }
}

void BusinessSocket::ngx_bussiness_register(TCPConnection * c, MsgHeader * msgHeader, uint8_t * pktBody, uint16_t pktBodyLen){
    ngx_log(NGX_LOG_INFO, 0, "处理注册逻辑");
}

void BusinessSocket::ngx_bussiness_login(TCPConnection * c, MsgHeader * msgHeader, uint8_t * pktBody, uint16_t pktBodyLen){
    ngx_log(NGX_LOG_INFO, 0, "处理登录逻辑");
}