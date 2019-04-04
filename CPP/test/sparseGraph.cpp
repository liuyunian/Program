#include <iostream>
#include <vector>
#include <cassert>

class SparseGraph{
private:
    int vNums, vEdges;
    bool isDirected;
    std::vector<std::vector<int>> g;

public:
    SparseGraph(int vN, bool isD) : vNums(vN), vEdges(0), isDirected(isD){
        for(int i = 0; i < vNums; i++){
            g.push_back(std::vector<int>());
        }
    }

    int getV(){
        return vNums;
    }

    int getE(){
        return vEdges;
    }

    void addEdge(int i, int j){
        assert(i >= 0 && i < vNums);
        assert(j >= 0 && j < vNums);

        g[i].push_back(j);

        if(i != j && !isDirected){
            g[j].push_back(i);
        }
        vEdges ++;
    }

    bool hasEdge(int i, int j){
        assert(i >= 0 && i < vNums);
        assert(j >= 0 && j < vNums);

        for(int k = 0; k < g[i].size(); k++){
            if(g[i][k] == j){
                return true;
            }
        }
        return false;
    }

    class Iterator{
    private:
        SparseGraph & m_Graph;
        int m_vertex;

        int m_index;
    public:
        Iterator(SparseGraph & g, int v) : 
            m_Graph(g), 
            m_vertex(v), 
            m_index(0){}

        int begin(){
            m_index = 0;
            if(m_Graph.g[m_vertex].size() > 0){
                return m_Graph.g[m_vertex][m_index];
            }
            return -1;
        }

        int next(){
            m_index ++;
            if(m_index < m_Graph.g[m_vertex].size()){
                return m_Graph.g[m_vertex][m_index];
            }
            return -1;
        }

        bool end(){
            return m_index >=  m_Graph.g[m_vertex].size();
        }
    };
};