#include <errno.h> // errno
#include <string.h> // memcpy
#include <sys/socket.h> // accpet accpet4

#include "ngx_func.h"
#include "ngx_macro.h"
#include "ngx_log.h"
#include "ngx_c_socket.h"
#include "ngx_c_memoryPool.h"

void Socket::ngx_event_accpet(TCPConnection * c){
    struct sockaddr cliAddr;
    socklen_t addrlen;
    int errnoCp; // 拷贝errno
    int sockfd; // TCP连接对应的socket描述符
    int accept4_flag; // 标记是否使用accept4，accept4函数是linux特有的扩展函数
    TCPConnection * newConnection; // 与新连接绑定

    addrlen = sizeof(struct sockaddr);
    while(1){
        if(accept4_flag){
            sockfd = accpet4(c->sockfd, &cliAddr, &addrlen, SOCK_NONBLOCK);
        }
        else{
            sockfd = accept(c->sockfd, &cliAddr, &addrlen);
        }

        if(sockfd == -1){ // 发生了错误
            errnoCp = errno;
            if(accept4_flag && errnoCp == ENOSYS){ // 不支持accept4
                accept4_flag = 0;
                continue;
            }

            if(errnoCp == EAGAIN){ // accept()没准备好，这个EAGAIN错误和EWOULDBLOCK是一样的
                //如何处理...
            }

            if(errnoCp == ECONNABORTED){ // 对方意外关闭套接字
                //如何处理...
            }

            if(errnoCp == EMFILE || errnoCp == ENFILE){ // EMFILE进程文件描述符用尽，ENFILE??
                //如何处理...
            }

            ngx_log(NGX_LOG_FATAL, errnoCp, "ngx_event_accpet()函数中执行accept()函数失败");
            return;
        }

        // 处理accept成功的情况
        if(!accept4_flag){
            if(ngx_set_nonblocking(sockfd) < 0){
                ngx_log(NGX_LOG_ERR, errno, "ngx_event_accpet()中执行ngx_set_nonblocking()失败");
                close(sockfd);                                             
                return;
            }
        }

        newConnection = m_connectionPool->ngx_get_connection();
        if(newConnection == nullptr){ // 直接错误返回，因为ngx_get_connection()函数中已经写过日志了
            close(sockfd);
            return;
        }
        // 需判断是否超过最大允许的连接数...

        memcpy(&newConnection->cliAddr, &cliAddr, addrlen);
        newConnection->w_ready = 1; // 已准备好写数据
        newConnection->r_handler = &Socket::ngx_event_recv;
        newConnection->curRecvPktState = RECV_PKT_HEADER; // 收包状态处于准备接收包头状态
        newConnection->recvIndex = newConnection->pktHeader;
        newConnection->recvLength = sizeof(PktHeader);
        
        if(ngx_epoll_addEvent(sockfd, 1, 0, 0, EPOLL_CTL_ADD, newConnection) < 0){ // LT模式
            ngx_event_close(newConnection);
            return;
        }

        break;
    }
}

void Socket::ngx_event_close(TCPConnection * c){
    close(c->sockfd);
    c->sockfd = -1; // 表征该连接已过期
    m_connectionPool->ngx_free_connection(c);
}

void Socket::ngx_event_recv(TCPConnection * c){
    ssize_t len = recv(c->sockfd, c->recvIndex, c->recvLength, 0); // recv()系统调用，这里flags设置为0，作用同read()

    if(len < 0){ // 出错，通过errno判断错误类型
        ngx_log(NGX_LOG_ERR, errno, "ngx_event_recv()函数中调用recv()函数接收数据错误");

        int errnoCp = errno;
        if(errnoCp == EAGAIN || errnoCp == EWOULDBLOCK){ // 表示没有拿到数据
            /*
            一般只有在ET模式下才会出现该错误
            该错误类型常出现在ET模式下，ET模式下从缓存区中拿数据是要在循环中不断的调用recv()
            当recv()函数返回-1，并且errno = EAGAIN || EWOULDBLOCK，表示缓冲区中没有数据了，此次接收数据结束
             */
            return;
        }

        if(errnoCp == EINTR){ // 什么错误？？
            // ... 如何处理
            return;
        }

        if(errnoCp == ECONNRESET){ // 客户端没有正常的关闭连接，此时服务器端也应关闭连接
            ngx_event_close(c);
            return;
        }

        // ... 其他错误等待后续完善
    }

    if(len == 0){ // 表示客户端关闭连接，服务器端也应关闭连接
        ngx_event_close(c);
        return;
    }

    // 处理正确接收的情况，采用状态机
    if(c->curRecvPktState == RECV_PKT_HEADER){
        if(len == c->recvLength){ // 接收到了完整的包头
            ngx_pktHead_handle(c);
        }
        else{
            c->recvIndex += len;
            c->recvLength -= len;
        }
    }
    else if(c->curRecvPktState == RECV_PKT_BODY){
        if(len == c->recvLength){ // 接收到了完整的包体
            ngx_pktBody_handle(c);
        }
        else{
            c->recvIndex += len;
            c->recvLength -= len;
        }
    }
    else{
        ngx_log(NGX_LOG_ERR, 0, "ngx_event_recv()函数中TCP连接对象c处于无效的收包状态，%d", c->curRecvPktState);
    }
}

void Socket::ngx_pktHeader_handle(TCPConnection * c){
    uint16_t pktHd_len = static_cast<uint16_t>(sizeof(PktHeader));

    PktHeader * pktHd = (PktHeader *)(c->pktHeader);
    uint16_t pktLen = ntohs(header->len); // 拿到完整的包长信息
    if(pktLen < pktHd_len){ // 记录的包长小于包头长度，此时数据包无效
        c->curRecvPktState = RECV_PKT_HEADER;
        c->recvIndex = c->pktHeader;
        c->recvLength = pktHd_len;

        // ...这里处理有问题，没有将缓冲区中包体丢弃
        return;
    }

    if(pktLen > PKT_MAX_LEN){ // 对于大于PKT_MAX_LEN(10240)字节的包，认为也是无效包
        c->curRecvPktState = RECV_PKT_HEADER;
        c->recvIndex = c->pktHeader;
        c->recvLength = pktHd_len;

        // ...这里处理有问题，没有将缓冲区中包体丢弃
        return;
    }

    // 处理正确的情况
    // [1] 分配内存，用于存放消息头+包头+包体
    uint8_t * buf = (uint8_t *)MemoryPool::getInstance()->ngx_alloc_memory(sizeof(struct MsgHeader) + pktLen);
    c->recvBuffer = buf;

    // [2] 填写消息头
    MsgHeader * msgHd = (MsgHeader *)buf;
    msgHd->c = c;
    msgHd->curSeq = c->curSeq;

    // [3] 填充包头
    buf += sizeof(MsgHeader);
    memcpy(buf, c->pktHeader, pktHd_len);

    if(pktLen < pktHd_len){ // 只有包头没有包体的情况，这种情况是允许的
        ngx_pkt_handle(c);
    }
    else{
        c->curRecvPktState = RECV_PKT_BODY;
        c->recvIndex = buf + pktHd_len; // buf当前指向包头，后移sizeof(PktHeader)指向包体
        c->recvLength = pktLen - pktHd_len;
    }
}

void Socket::ngx_pkt_handle(TCPConnection * c){
    // [1] 将接收到的完整的数据push到消息队列
    ngx_msgQue_push(c->recvBuffer);

    // [2] 触发业务逻辑
    // ...后续增加

    // [3] 准备接收后续的数据
    c->recvBuffer = nullptr; // 对应的内存由消息队列接管
    c->curRecvPktState = RECV_PKT_HEADER;
    c->recvIndex = c->pktHeader;
    c->recvLength = sizeof(PktHeader);
}

void Socket::ngx_msgQue_push(uint8_t * msg){
    // 因业务逻辑要引入多线程，这里要注意临界问题
    m_recvMsgQueue.push_back(msg);
}

void Socket::ngx_msgQue_clear(){
    MemoryPool * mp = MemoryPool::getInstance();
    uint8_t * msg = nullptr;
    while(!m_recvMsgQueue.empty()){
        msg = m_recvMsgQueue.front();
        m_recvMsgQueue.pop_front(); // 移除队首的消息
        mp->ngx_free_memory(msg);
    }
    
    m_recvMsgQueue.clear();
}