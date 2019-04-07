#ifndef DENSEGRAPH_H_
#define DENSEGRAPH_H_

#include <iostream>
#include <cassert>
#include <vector>

class DenseGraph{
private:
    int vNums, eNums;
    bool isDirected;
    std::vector<std::vector<bool>> g;

public:
    DenseGraph(int vN, bool isD) : vNums(vN), eNums(0), isDirected(isD){
        for(int i = 0; i < vNums; i++){
            g.push_back(std::vector<bool>(vNums, false));
        }
    }

    ~DenseGraph(){}

    int getV(){
        return vNums;
    }

    int getE(){
        return eNums;
    }

    void addEdge(int i, int j){
        assert(i >= 0 && i < vNums);
        assert(j >= 0 && j < vNums);

        if(hasEdge(i, j)){ //处理平行边
            return;
        }

        g[i][j] = true;
        if(!isDirected){
            g[j][i] = true;
        }
        eNums ++;
    }

    bool hasEdge(int i, int j){
        assert(i >= 0 && i < vNums);
        assert(j >= 0 && j < vNums);

        return g[i][j];
    }

    void printDenseGraph(){
        for(int i = 0; i < vNums; i++){
            for(int j = 0; j < vNums; j++){
                std::cout << g[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

    class adjIterator{
    private:
        DenseGraph & m_Graph;
        int m_Vertex;
        
        int m_index;

    public:
        adjIterator(DenseGraph & g, int v) : 
            m_Graph(g), 
            m_Vertex(v),
            m_index(-1){}

        int begin(){
            m_index = -1;
            return next();
        }

        int next(){
            for(m_index += 1; m_index < m_Graph.g[m_Vertex].size(); m_index ++){
                if(m_Graph.g[m_Vertex][m_index]){
                    return m_index;
                }
            }
            return -1;
        }

        bool end(){
            return m_index >= m_Graph.g[m_Vertex].size();
        }
    };
};

#endif