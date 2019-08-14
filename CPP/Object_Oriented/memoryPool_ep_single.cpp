/**
 * 将嵌入式指针实现类内内存池提取出构造一个可以复用的类MemoryPool
 */
#include <iostream>
#include <vector>

#include <stdio.h>

class MemoryPool{
public:
    MemoryPool(int chunkCount) : 
        m_chunkCount(chunkCount),
        m_free(nullptr){}

    ~MemoryPool(){
        for(auto &item : m_memoryVec){
            free(item);
        }
    }

    void * mp_malloc(size_t size){
        EP * tmp;
        if(m_free == nullptr){
            m_free = static_cast<EP *>(malloc(size * m_chunkCount)); // 申请分配一大块内存
            m_memoryVec.push_back(m_free);
            tmp = m_free;

            for (int i = 0; i < m_chunkCount; ++ i){
                tmp->next = (EP *)((char *)tmp + size);
                tmp = tmp->next;
            }

            tmp->next = nullptr;
        }

        tmp = m_free;
        m_free = m_free->next;
        return tmp;
    }

    void mp_free(void * p){
        EP * tmp = static_cast<EP *>(p);
        tmp->next = m_free;
        m_free = tmp;
    }

private:
    struct EP{
        EP * next;
    };

    EP * m_free; // 空闲内存的首地址
    int m_chunkCount; // 以单个对象所占用的内存作为块，这里记录malloc()申请多少块内存
    std::vector<EP *> m_memoryVec;
};

class A{
public:
    A();
    ~A();

    static void * operator new(size_t size);

    static void operator delete(void * p);

private:
    static MemoryPool m_mp;
    int m_i;
    int m_j; // 这两个成员变量是为了满足应用嵌入式指针的条件--对象内存 > 指针所占的空间
};

MemoryPool A::m_mp(50);

A::A(){
    // std::cout << "A类中的构造函数被调用" << std::endl;
}

A::~A(){
    // std::cout << "A类中的析构函数被调用" << std::endl;
}

void * A::operator new(size_t size){
    // std::cout << "A类中的重载new操作符函数被调用" << std::endl;
    return m_mp.mp_malloc(size);
}

void A::operator delete(void * p){
    // std::cout << "A类中的重载delete操作符函数被调用" << std::endl;
    m_mp.mp_free(p);
}

int main(){
    A * p_as[15];
    for(int i = 0; i < 15; ++ i){
        p_as[i] = new A();
        printf("%p\n", p_as[i]);
    }

    for(int i = 0; i < 15; ++ i){
        delete p_as[i];
    }

    return 0;
}