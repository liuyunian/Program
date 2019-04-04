#include <iostream>
#include "distNode.h"
#include "node.h"

int main(){
    Node * node = new DistNode(1);
    delete node;

    return 0;
}