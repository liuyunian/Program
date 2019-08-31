#include <iostream>

#include "Exception.h"

class Bar{
public:
    void test(){
        throw Exception("oops");
    }
};

void foo(){
    Bar b;
    b.test();
}

int main(){
    try{
        foo();
    }
    catch(Exception & e){
        std::cout << e.what() << std::endl;
        std::cout << e.stack_trace() << std::endl; 
    }
    
    return 0;
}