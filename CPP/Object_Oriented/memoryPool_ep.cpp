/**
 * 采用嵌入式指针改进针对一个类的内存池
 */
#include <iostream>
#include <vector>

class A{
public:
    A();
    ~A();

    static void * operator new(size_t size);

    static void operator delete(void * p);

private:
    struct EP{
        EP * next;
    };

public:
    static std::vector<EP *> m_memoryVec;

    int m_i;
    int m_j; // 这两个成员变量是为了满足应用嵌入式指针的条件--对象内存 > 指针所占的空间

private:
    static EP * m_free; // 空闲内存的首地址
    static int m_chunkCount; // 以单个对象所占用的内存作为块，这里记录malloc()申请多少块内存
};

A::EP * A::m_free = nullptr;
int A::m_chunkCount = 50;
std::vector<A::EP *> A::m_memoryVec;

A::A(){
    std::cout << "A类中的构造函数被调用" << std::endl;
}

A::~A(){
    std::cout << "A类中的析构函数被调用" << std::endl;
}

void * A::operator new(size_t size){
    std::cout << "A类中的重载new操作符函数被调用" << std::endl;

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

void A::operator delete(void * p){
    std::cout << "A类中的重载delete操作符函数被调用" << std::endl;

    EP * tmp = static_cast<EP *>(p);
    tmp->next = m_free;
    m_free = tmp;
}

int main(){
    A * p_a = new A();
    p_a->m_i = 0;
    p_a->m_j = 1;

    delete p_a;

    for(auto &item : A::m_memoryVec){
        free(item);
    }

    return 0;
}