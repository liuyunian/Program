#include <string.h> // memset

#include "ngx_c_memoryPool.h"

MemoryPool * MemoryPool::instance = nullptr;

MemoryPool::MemoryPool(){}

MemoryPool::~MemoryPool(){}

void * MemoryPool::ngx_alloc_memory(size_t size){
    void * tmp = static_cast<void *>(new char[size]);
    memset(tmp, 0, size); // 所分配的堆内存置0
    return tmp;
}

void MemoryPool::ngx_free_memory(void * mem){
    char * tmp = static_cast<char *>(mem);
    delete[] tmp;
}
