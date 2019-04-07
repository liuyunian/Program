#include <iostream>

class A{
private:
    static A * instance;

    A(){}
    ~A(){}

    class GCInstance{
    public:
        ~GCInstance(){
            if(A::instance != nullptr){
                delete A::instance;
                A::instance = nullptr;
            }
        }
    };

public:
    static A * getInstance(){
        if(instance != nullptr){
            instance = new A();
            static GCInstance gc;
        }
        return instance;
    }
};

A * A::instance = nullptr;