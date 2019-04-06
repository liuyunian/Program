#include <iostream>
#include <cassert>

template <typename Graph>
class Commponent{
private:
    Graph & m_graph;
    bool * m_visited;
    int * m_id;
    int m_ccount; //连通分量的个数

    void depthFirstTraversal(int v){
        m_visited[v] = true;

        m_id[v] = m_ccount;
        typename Graph::adjIterator adj(m_graph, v);
        for(int i = adj.begin(); !adj.end(); i = adj.next()){
            if(m_visited[i] == false){
                depthFirstTraversal(i);
            }
        }

    } 

public:
    Commponent(Graph & graph) : m_graph(graph), m_ccount(0){
        m_visited = new bool[m_graph.getV()];
        m_id = new int[m_graph.getV()];
        for(int i = 0; i < m_graph.getV(); i++){
            m_visited = false;
            m_id[i] = i;
        }

        for(int i = 0; i < m_graph.getV(); i++){
            if(m_visited[i] == false){
                depthFirstTraversal(i);
                m_ccount ++;
            }
        }
    }

    ~Commponent(){
        delete[] m_visited;
    }

    int getCommponent(){
        return m_ccount;
    }

    bool isConnected(int i, int j){
        assert(i >= 0 && i < m_graph.getV());
        assert(j >= 0 && j < m_graph.getV());

        return m_id[i] == m_id[j];
    }

};