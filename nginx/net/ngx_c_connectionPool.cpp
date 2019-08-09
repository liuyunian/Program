#include "ngx_macro.h"
#include "ngx_func.h"
#include "ngx_c_connectionPool.h"

ConnectionPool::ConnectionPool(int size) : 
    m_poolSize(size),
    m_freeSize(size),
    m_connectionPool(nullptr),
    m_freeConnectionPool(nullptr)
{
    m_connectionPool = new TCPConnection[m_poolSize];
    TCPConnection * next = nullptr;
    for(int i = m_poolSize-1; i >= 0; -- i){
        m_connectionPool[i].next = next;
        m_connectionPool[i].sockfd = -1;
        m_connectionPool[i].instance = 1;
        m_connectionPool[i].curSeq = 0;

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
        log(NGX_LOG_ERR, 0, "连接池中不存在空闲的连接");
        return nullptr;
    }

    m_freeConnectionPool = c->next;
    -- m_freeSize;

    // (1)注意这里的操作,先把c指向的对象中有用的东西搞出来保存成变量，因为这些数据可能有用
    uintptr_t instance = c->instance;   //常规c->instance在刚构造连接池时这里是1【失效】
    uint64_t curSeq = c->curSeq;
    //....其他内容再增加


    //(2)把以往有用的数据搞出来后，清空并给适当值
    memset(c, 0, sizeof(TCPConnection));
    c->sockfd = sockfd;
    //....其他内容再增加

    //(3)这个值有用，所以在上边(1)中被保留，没有被清空，这里又把这个值赋回来
    c->instance = !instance;
    c->curSeq = curSeq+1;  //每次取用该值都增加1

    return c; 
}

void ConnectionPool::ngx_free_connection(TCPConnection * c){
    c->next = m_freeConnectionPool;

    ++ c->curSeq;

    m_freeConnectionPool = c;
    ++ m_freeSize;
}

