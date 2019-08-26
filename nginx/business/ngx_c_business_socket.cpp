#include <string.h> // memcpy

#include "ngx_c_business_socket.h"

#include "_include/ngx_func.h"
#include "_include/ngx_macro.h"
#include "_include/ngx_global.h"
#include "net/ngx_c_connectionPool.h"
#include "misc/ngx_c_memoryPool.h"

/**
 * 客户端发来的包中有类型字段，服务器端根据类型值确定该数据包如何处理
 * 这里实现类型与处理逻辑对应关系是通过下标和函数指针数组实现的，类型值作为数组下标从函数指针数组中找到对应的处理函数
 * 比起if语句判断，这种方式更加高效，代码可读性更高
 * 这种思路就要求将具体的处理函数统一格式，这样才可以放入数组
*/
typedef void (BusinessSocket:: * handler)(
    MsgHeader * msgHeader,  // 消息头
    uint8_t * pktBody,      // 包体
    uint16_t pktBodyLen     // 包体长度
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

static const int maxValidType = sizeof(typeHandlers) / sizeof(handler); // 可以进行处理的最大有效类型

BusinessSocket::BusinessSocket(){}

BusinessSocket::~BusinessSocket(){}

// 覆盖从父类继承的函数
int BusinessSocket::ngx_socket_master_init(){
    // ... BusinessSocket中需要添加的内容

    return Socket::ngx_socket_master_init(); // 调用父类的ngx_socket_master_init()函数
}

int BusinessSocket::ngx_socket_worker_init(){
    // ... BusinessSocket中需要添加的内容

    return Socket::ngx_socket_worker_init(); // 调用父类的ngx_socket_worker_init()函数
}


void BusinessSocket::ngx_socket_master_destroy(){
    // ... BusinessSocket中需要添加的内容

    Socket::ngx_socket_master_destroy(); // 调用父类的ngx_socket_master_destroy()函数
}

void BusinessSocket::ngx_socket_worker_destroy(){
    // ... BusinessSocket中需要添加的内容

    Socket::ngx_socket_worker_destroy(); // 调用父类的ngx_socket_worker_destroy()函数
}

void BusinessSocket::ngx_recvMsg_handle(uint8_t * msg){
    MsgHeader * msgHeader = (MsgHeader *)msg;
    TCPConnection * c = msgHeader->c;

    // 判断该消息是否过期，消息在消息队列中停留时，如果该消息对应的连接断开，那么该消息就过期了，不进行处理
    // 这里运行于线程池中的线程，并访问了TCPConnection对象，是否需要临界？？？？
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
        (this->*typeHandlers[type])(msgHeader, pktBody, pktBodyLen);
    }
}

void BusinessSocket::ngx_bussiness_register(MsgHeader * msgHeader, uint8_t * pktBody, uint16_t pktBodyLen){
    // ngx_log(NGX_LOG_INFO, 0, "处理注册逻辑");

    // [1] 判断包体（注册信息）的合法性
    if(pktBody == nullptr){ // 注册逻辑中包体不能为空
        return;
    }

    if(pktBodyLen != sizeof(RegisterInfo)){ // 长度与结构不符
        return;
    }

    // [2] 拿到合法的包体数据之后，要进行实际的业务处理
    RegisterInfo * rInfor = (RegisterInfo *)pktBody;
    // 是否要判断注册信息的合法性
    // 是否写入数据库
    // ...

    /**
     * 关于实际的业务逻辑处理这块，老师的课程中使用了互斥量来保证一次只能处理同一个用户的一次操作
     * 老师举的例子是：网游中购买物品，用户有100金币，物品A80金币，物品B90金币，那么如果用户同时发来购买A和B两个请求，如果两个请求消息同时处理的话可能会导致100金币同时购买了A和B两个药品，这就是程序的BUG
     * 个人认为这两个请求中的临界区用户的账号，应该对账号进行互斥，而不应该对用户请求限制同一时间只能处理一个请求
     * 还有就是用户的请求可能是相互独立的，完全可以同时处理，以提高程序效率
    */

    // [3] 给客户端发送回应数据
    /**
     * 回应数据的格式也是包头+包体
     * 包头的格式是需要根据具体的业务确定的（客户端向服务器端发送的数据包的包头同样也是如此），两个方向的包头不一定是一样的
     * 这里为了方便，采用客户端向服务器端发送的数据包的包头
     * 
     * 包体的格式也是需要客户端与服务器端协商的
     * 这里为了测试方便，包体定为1024字节的空数据
     * 
     * 另外也为回应数据添加了消息头
    */
    const int PKT_BODY_SZ = 1024; // 临时规定，实际还是需要更加具体的业务逻辑确定

    MemoryPool * mp = MemoryPool::getInstance();
    uint8_t * sendMsg = (uint8_t *)mp->ngx_alloc_memory(MSG_HEADER_SZ + PKT_HEADER_SZ + PKT_BODY_SZ);

    // 填充消息头
    memcpy(sendMsg, msgHeader, MSG_HEADER_SZ);

    // 填充包头
    PktHeader * ph = (PktHeader *)(sendMsg + MSG_HEADER_SZ);
    ph->len = htons(PKT_HEADER_SZ + PKT_BODY_SZ);
    ph->type = htons(5); // 5表示注册

    // 填充包体
    // ... 更加具体逻辑添加这里为1024字节0（分配内存时有对内存清0）

    // 填充CRC32校验值
    int crc32 = ngx_crc32_get_value(sendMsg + MSG_HEADER_SZ + PKT_HEADER_SZ, PKT_BODY_SZ);
    ph->crc32 = htonl(crc32);

    // 发送数据
    ngx_sendMsgQue_push(sendMsg);
}

void BusinessSocket::ngx_bussiness_login(MsgHeader * msgHeader, uint8_t * pktBody, uint16_t pktBodyLen){
    ngx_log(NGX_LOG_INFO, 0, "处理登录逻辑");
}