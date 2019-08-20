#include <stdint.h> // uintptr_t

#include "ngx_c_connectionPool.h"

#include "app/ngx_log.h"
#include "_include/ngx_macro.h"
#include "_include/ngx_func.h"
#include "misc/ngx_c_memoryPool.h"

ConnectionPool::ConnectionPool(int size) : 
    m_poolSize(size),
    m_freeSize(size),
    m_connectionPool(nullptr),
    m_free(nullptr),
    m_tail(nullptr)
{
    m_connectionPool = new TCPConnection[m_poolSize];
    TCPConnection * next = nullptr;
    for(int i = m_poolSize-1; i >= 0; -- i){
        m_connectionPool[i].validFlag = 1;
        m_connectionPool[i].next = next;
        next = &m_connectionPool[i];
    }
    
    m_free = m_connectionPool; // 数组中第一个连接对象
    m_tail = &m_connectionPool[m_poolSize-1]; // 数组中的最后一个连接对象
}

ConnectionPool::~ConnectionPool(){
    if(m_connectionPool != nullptr){
        delete[] m_connectionPool;
    }
}

TCPConnection * ConnectionPool::ngx_get_connection(int sockfd){
    TCPConnection * c = m_free; // m_freeConnectionPool空闲连接的头部
    if(c == nullptr){
        ngx_log(NGX_LOG_ERR, 0, "连接池中不存在空闲的连接");
        return nullptr;
    }

    m_free = c->next;
    -- m_freeSize;

    // [1] 将旧连接对象中的有用的数据暂时保存到变量中 
	uintptr_t validFlag = c->validFlag; 
    uint64_t curSeq = c->curSeq;

    // [2] 清空并赋值
    memset(c, 0, sizeof(TCPConnection));
    c->sockfd = sockfd;
    c->validFlag = !validFlag; // 取反，用于判断事件是否过期
    c->curSeq = curSeq+1;  // 每次取用该值都增加1
    if(c->curSeq == UINT64_MAX){
        c->curSeq = 0;
    }
    //....其他内容再增加

    return c; 
}

void ConnectionPool::ngx_free_connection(TCPConnection * c){
    if(c->recvBuffer != nullptr){
        MemoryPool::getInstance()->ngx_free_memory(c->recvBuffer);
        c->recvBuffer = nullptr;
    }

    ++ (c->curSeq); // 释放连接时也++
    if(c->curSeq == UINT64_MAX){
        c->curSeq = 0;
    }
    c->next = m_tail->next;
    m_tail = c;
    ++ m_freeSize;
}

