#ifndef DENSEGRAPH_H_
#define DENSEGRAPH_H_

#include <iostream>
#include <cassert>
#include <vector>

#include "edge.h"

template <typename Weight>
class DenseGraph{
private:
    int vNums, eNums;
    bool isDirected;
    std::vector<std::vector<Edge<Weight> *>> g;

public:
    DenseGraph(int vN, bool isD) : vNums(vN), eNums(0), isDirected(isD){
        for(int i = 0; i < vNums; i++){
            g.push_back(std::vector<Edge<Weight> *>(vNums, nullptr));
        }
    }

    ~DenseGraph(){
        for(int i = 0; i < vNums; i++){
            for(int j = 0; j < vNums; j++){
                if(g[i][j] != nullptr){
                    delete g[i][j];
                }
            }
        }
    }

    int getV(){
        return vNums;
    }

    int getE(){
        return eNums;
    }

    void addEdge(int i, int j, Weight w){
        assert(i >= 0 && i < vNums);
        assert(j >= 0 && j < vNums);

        if(hasEdge(i, j)){ //处理平行边
            delete g[i][j];

            if(!isDirected){
                delete g[j][i];
            }

            -- eNums;
        }

        g[i][j] = new Edge<Weight>(i, j, w);

        if(!isDirected){
            g[j][i] = new Edge<Weight>(j, i, w);;
        }

        ++ eNums;
    }

    bool hasEdge(int i, int j){
        assert(i >= 0 && i < vNums);
        assert(j >= 0 && j < vNums);

        return g[i][j] != nullptr;
    }

    void printDenseGraph(){
        for(int i = 0; i < vNums; i++){
            for(int j = 0; j < vNums; j++){
                if(g[i][j] != nullptr){
                    std::cout << g[i][j]->getWeight() << " ";
                }
                else{
                    std::cout << "nullptr" << " ";
                }
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

        Edge<Weight> * begin(){
            m_index = -1;
            return next();
        }

        Edge<Weight> * next(){
            for(m_index += 1; m_index < m_Graph.g[m_Vertex].size(); m_index ++){
                if(m_Graph.g[m_Vertex][m_index] != nullptr){
                    return m_Graph.g[m_Vertex][m_index];
                }
            }
            return nullptr;
        }

        bool end(){
            return m_index >= m_Graph.g[m_Vertex].size();
        }
    };
};

#endif