#ifndef READGRAPH_H
#define READGRAPH_H

#include <fstream>
#include <cassert>
#include <sstream>

template <typename Graph>
class ReadGraph{
public:
    ReadGraph(Graph & graph, const std::string & fileName){
        std::ifstream fin;
        std::string line;
        int vNums, eNums;

        fin.open(fileName);
        assert(fin.is_open());

        assert(std::getline(fin, line));
        std::stringstream ss(line);
        ss >> vNums >> eNums;

        assert(vNums == graph.getV());
        for(int i = 0; i < eNums; i ++){
            assert(std::getline(fin, line));
            std::stringstream ss(line);

            int a, b;
            ss >> a >> b;
            assert(a >= 0 && a < vNums);
            assert(b >= 0 && b < vNums);
            graph.addEdge(a, b);
        }

    }
};

#endif