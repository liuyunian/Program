#ifndef SHORTEST_H_
#define SHORTEST_H_

template<typename Graph>
class ShortestPath{
private:
    Graph m_graph;
    bool * m_visited;
    int * m_from;
    int * m_ord;
    int m_source;

public:
    ShortestPath(Graph & graph, int s) : m_graph(graph), m_source(s){\
        assert(s >= 0 && s < m_graph.getV());
        this->m_source = s;

        m_visited = new bool[m_graph.getV()];
        m_from = new int[m_graph.getV()];
        m_ord = new int[m_graph.getV()];
        for(int i = 0; i < m_graph.getV(); i ++){
            m_visited[i] = false;
            m_from[i] = -1;
            m_ord[i] = -1;
        }

        std::queue<int> q;
        q.push(m_source);
        m_visited[m_source] = true;
        m_ord[m_source] = 0;
        while(!q.empty()){
            int v = q.front();
            q.pop();

            typename Graph::adjIterator adj(m_graph, v);
            for(int i = adj.begin(); !adj.end(); i = adj.next()){
                if(m_visited[i] == false){
                    q.push(i);
                    m_visited[i] = true;
                    m_from[i] = v;
                    m_ord[i] = m_ord[v]+1;
                }
            }
        }
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

    int getPathLength(int d){
        assert(d >= 0 && d < m_graph.getV());

        return m_ord[d];
    }
};

#endif