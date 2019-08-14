/**
 * 用于放置Socket类中与消息队列有关的成员函数
 */

#include <pthread.h> // pthread_mutex_lock pthread_mutex_unlock

#include "ngx_c_socket.h"
#include "ngx_c_memoryPool.h"

void Socket::ngx_msgQue_push(uint8_t * msg){
    pthread_mutex_lock(&m_msgQueMutex);
    m_msgQueue.push_back(msg);

    pthread_mutex_unlock(&m_msgQueMutex);
}

uint8_t * Socket::ngx_msgQue_pop(){
    pthread_mutex_lock(&m_msgQueMutex);
    if(m_msgQueue.empty()){
        pthread_mutex_unlock(&m_msgQueMutex);
        return nullptr;
    }

    uint8_t * msg = msg = m_msgQueue.front();
    m_msgQueue.pop_front(); // 移除队首的消息
    pthread_mutex_unlock(&m_msgQueMutex);

    return msg;
}

void Socket::ngx_msgQue_clear(){
    MemoryPool * mp = MemoryPool::getInstance();
    uint8_t * msg = nullptr;
    pthread_mutex_lock(&m_msgQueMutex);
    while(!m_msgQueue.empty()){
        msg = m_msgQueue.front();
        m_msgQueue.pop_front(); // 移除队首的消息
        mp->ngx_free_memory(msg);
    }
    
    pthread_mutex_unlock(&m_msgQueMutex);
}