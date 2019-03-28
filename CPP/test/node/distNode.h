#ifndef DISTNODE_H_
#define DISTNODE_H_
#include "node.h"

class DistNode : public Node{
private:
    int data;
public:
    DistNode(std::string n, int d) : data(d){}
};

#endif