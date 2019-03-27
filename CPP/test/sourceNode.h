#ifndef SOURCENODE_H_
#define SOURCENODE_H_
#include "node.h"

class SourceNode : public Node{
public:
    SourceNode();

    using Node::getName;

    void getName(int a){
        Node::getName();
        Node::getName(1);

        std::cout << "SourceNode::getName(int a)" << std::endl;
    }
};

#endif