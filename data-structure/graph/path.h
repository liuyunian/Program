#ifndef PATH_H_
#define PATH_H_

#include <iostream>
#include <cassert>

template <typename Graph>
class Path{
private:
    Graph m_graph;
    bool * m_visited;
    int * m_from;
    int m_source;

    void depthFirstTraversal(int v){
        m_visited[v] = true;

        typename Graph::adjIterator adj(m_graph, v);
        for(int i = adj.begin(); !adj.end(); i = adj.next()){
            if(m_visited[i] == false){
                m_from[i] = v;
                depthFirstTraversal(i);
            }
        }
    } 

public:
    Path(Graph & graph, int s) : m_graph(graph){
        assert(s >= 0 && s < m_graph.getV());
        this->m_source = s;

        m_visited = new bool[m_graph.getV()];
        m_from = new int[m_graph.getV()];

        for(int i = 0; i < m_graph.getV(); i ++){
            m_visited[i] = false;
            m_from[i] = -1;
        }

        depthFirstTraversal(m_source);
    }

    bool hasPath(int d){
        assert(d >= 0 && d < m_graph.getV());

        return m_visited[d];
    }

    void getPath(int d, std::vector<int> & path){
        assert(d >= 0 && d < m_graph.getV());

        std::stack<int> s;
        int p = d;
        while(p != -1){
            s.push(p);
            p = from[p];
        }

        path.clear();
        while(!s.empty()){
            path.push_back(s.top());
            s.pop();
        }
    }

    void showPath(int d){
        assert(d >= 0 && d < m_graph.getV());

        std::vector<int> path;
        getPath(d, path);

        for(int i = 0; i < path.size()-1; i++){
            std::cout << path[i] << " -> ";
        }
        std::cout << path[path.size()-1] << std::endl;
    }
};

#endif