#include <stdint.h> // uintptr_t

#include "ngx_macro.h"
#include "ngx_func.h"
#include "ngx_log.h"
#include "ngx_c_connectionPool.h"
#include "ngx_c_memoryPool.h"

ConnectionPool::ConnectionPool(int size) : 
    m_poolSize(size),
    m_freeSize(size),
    m_connectionPool(nullptr),
    m_freeConnectionPool(nullptr)
{
    m_connectionPool = new TCPConnection[m_poolSize];
    TCPConnection * next = nullptr;
    for(int i = m_poolSize-1; i >= 0; -- i){
        m_connectionPool[i].instance = 1;
        m_connectionPool[i].next = next;
        next = &m_connectionPool[i];
    }

    m_freeConnectionPool = m_connectionPool;
}

ConnectionPool::~ConnectionPool(){
    if(m_connectionPool != nullptr){
        delete[] m_connectionPool;
    }
}

TCPConnection * ConnectionPool::ngx_get_connection(int sockfd){
    TCPConnection * c = m_freeConnectionPool; // m_freeConnectionPool空闲连接的头部
    if(c == nullptr){
        ngx_log(NGX_LOG_ERR, 0, "连接池中不存在空闲的连接");
        return nullptr;
    }

    m_freeConnectionPool = c->next;
    -- m_freeSize;

    // [1] 将旧连接对象中的有用的数据暂时保存到变量中 
	uintptr_t instance = c->instance;
    uint64_t curSeq = c->curSeq;

    // [2] 清空并赋值
    memset(c, 0, sizeof(TCPConnection));
    c->sockfd = sockfd;
    c->instance = !instance; // 取反，用于判断事件是否过期
    c->curSeq = curSeq+1;  // 每次取用该值都增加1
    //....其他内容再增加

    return c; 
}

void ConnectionPool::ngx_free_connection(TCPConnection * c){
    if(c->recvBuffer != nullptr){
        MemoryPool::getInstance()->ngx_free_memory(c->recvBuffer);
        c->recvBuffer = nullptr;
    }

    c->next = m_freeConnectionPool;

    ++ (c->curSeq); // 释放连接时也++

    m_freeConnectionPool = c;
    ++ m_freeSize;
}

