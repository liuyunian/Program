/**
 * 针对一个类的内存池
 * 对于该类使用new和delete创建和释放对象时所需要的内存都从内存池中分配，而非直接malloc申请内存
 */
#include <iostream>
#include <vector>

#define MEMORYPOOL 1

class A{
public:
    A();
    ~A();

    static void * operator new(size_t size);

    static void operator delete(void * p);

    static std::vector<A *> m_memoryVec;

private:
    static A * m_free; // 空闲内存的首地址
    static int m_chunkCount; // 以单个对象所占用的内存作为块，这里记录malloc()申请多少块内存
    
    A * next;
};

A * A::m_free = nullptr;
int A::m_chunkCount = 50;
std::vector<A *> A::m_memoryVec;

A::A(){
    std::cout << "A类中的构造函数被调用" << std::endl;
}

A::~A(){
    std::cout << "A类中的析构函数被调用" << std::endl;
}

void * A::operator new(size_t size){
    std::cout << "A类中的重载new操作符函数被调用" << std::endl;
#ifndef MEMORYPOOL
    A * p_a = static_cast<A *>(malloc(size));
    return p_a;
#endif
    A * tmp;
    if(m_free == nullptr){
        m_free = static_cast<A *>(malloc(size * m_chunkCount)); // 申请分配一大块内存
        m_memoryVec.push_back(m_free);
        tmp = m_free;
        while(tmp != &m_free[m_chunkCount-1]){
            tmp->next = tmp + 1;
            ++ tmp;
        }
    }

    tmp = m_free;
    m_free = m_free->next;
    return tmp;
}

void A::operator delete(void * p){
    std::cout << "A类中的重载delete操作符函数被调用" << std::endl;
#ifndef MEMORYPOOL
    free(p);
#endif
    A * tmp = static_cast<A *>(p);
    tmp->next = m_free;
    m_free = tmp;
}

int main(){
    A * p_a = new A();
    delete p_a;

    for(auto &item : A::m_memoryVec){
        free(item);
    }

    return 0;
}