#include <iostream>
#include "distNode.h"
#include "node.h"

int main(){
    DistNode dn("/ndn", 1);
    dn.printName();
    std::cout << dn.getData() << std::endl;
    return 0;
}