#ifndef DISTNODE_H_
#define DISTNODE_H_
#include "node.h"

class DistNode : public Node{
public:
    DistNode();

    ~DistNode();

    void print(std::string str);

    using Node::print;
};

#endif