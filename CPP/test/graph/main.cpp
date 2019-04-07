#include <iostream>
#include "readGraph.h"
#include "denseGraph.hpp"
#include "sparseGraph.hpp"

int main(){
    std::string fileName = "test.txt";
    SparseGraph sg(5, false);

    ReadGraph<SparseGraph> rg1(sg, fileName);
    sg.printSparseGraph();

    std::cout << std::endl;

    DenseGraph dg(5, false);
    ReadGraph<DenseGraph> rg2(dg, fileName);
    dg.printDenseGraph();

    return 0;
}