/**
 * 内存池，这里暂不实现，先用运算符new、delete代替
 * 
 */
#ifndef NGX_C_MEMORYPOOL_H_
#define NGX_C_MEMORYPOOL_H_

#include <sys/types.h> // size_t有符号 = long；ssize_t无符号 = unsigned long

class MemoryPool{
private:
    MemoryPool();

    ~MemoryPool();

    static MemoryPool * instance;

    class GCInstance{
    public:
        ~GCInstance(){
            if(MemoryPool::instance != nullptr){
                delete MemoryPool::instance;
                MemoryPool::instance = nullptr;
            }
        }
    };

public:
    static MemoryPool * getInstance(){
        if(instance == nullptr){
            instance = new MemoryPool;
            static GCInstance gc;
        }

        return instance;
    }

    /**
     * @brief 分配内存
     * @param size 分配内存的大小
     * @return 返回所分配内存的首地址
     */
    void * ngx_alloc_memory(size_t size);

    /**
     * @brief 释放内存
     * @param mem 要释放内存的首地址
     */
    void ngx_free_memory(void * mem);
};

#endif // NGX_C_MEMORYPOOL_H_