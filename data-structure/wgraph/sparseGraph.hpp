#ifndef SPARSEGRAPH_H_
#define SPARSEGRAPH_H_

#include <iostream>
#include <vector>
#include <cassert>

#include "edge.h"

template<typename Weight>
class SparseGraph{
private:
    int vNums, vEdges;
    bool isDirected;
    std::vector<std::vector<Edge<Weight> *>> g;

public:
    SparseGraph(int vN, bool isD) : vNums(vN), vEdges(0), isDirected(isD){
        for(int i = 0; i < vNums; i++){
            g.push_back(std::vector<Edge<Weight> *>());
        }
    }

    ~SparseGraph(){
        for(int i = 0; i < vNums; i++){
            for(int j = 0; j < g[i].size(); j++){
                delete g[i][j];
            }
        }
    }

    int getV(){
        return vNums;
    }

    int getE(){
        return vEdges;
    }

    void addEdge(int i, int j, Weight w){
        assert(i >= 0 && i < vNums);
        assert(j >= 0 && j < vNums);

        g[i].push_back(new Edge<Weight>(i, j, w));

        if(i != j && !isDirected){
            g[j].push_back(new Edge<Weight>(j, i, w));
        }
        vEdges ++;
    }

    bool hasEdge(int i, int j){
        assert(i >= 0 && i < vNums);
        assert(j >= 0 && j < vNums);

        for(int k = 0; k < g[i].size(); k++){
            if(g[i][k]->other() == j){
                return true;
            }
        }
        return false;
    }

    void printSparseGraph(){
        for(int i = 0; i < vNums; i++){
            std::cout << "vertex " << i << ": ";

            for(int j = 0; j < g[i].size(); j++){
                std::cout << "(to: " << g[i][j]->getD() << ", wt: " << g[i][j]->getWeight() << ") ";
            }

            std::cout << std::endl;
        }
    }

    class adjIterator{
    private:
        SparseGraph & m_Graph;
        int m_vertex;

        int m_index;
    public:
        adjIterator(SparseGraph & g, int v) : 
            m_Graph(g), 
            m_vertex(v), 
            m_index(0){}

        Edge<Weight> * begin(){
            m_index = 0;
            if(m_Graph.g[m_vertex].size() > 0){
                return m_Graph.g[m_vertex][m_index];
            }
            return nullptr;
        }

        Edge<Weight> * next(){
            m_index ++;
            if(m_index < m_Graph.g[m_vertex].size()){
                return m_Graph.g[m_vertex][m_index];
            }
            return nullptr;
        }

        bool end(){
            return m_index >=  m_Graph.g[m_vertex].size();
        }
    };
};

#endif