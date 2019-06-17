#ifndef EDGE_H_
#define EDGE_H_

template <typename Weight>
class Edge{
private:
    int m_s, m_d; //哪两个顶点之间的边
    Weight m_weight;

public:
    Edge(int s, int d, Weight weight) : m_s(s), m_d(d), m_weight(weight){}

    Edge(){}

    ~Edge(){}

    int getS(){
        return m_s;
    }

    int getD(){
        return m_d;
    }

    Weight getWeight(){
        return m_weight;
    }

    int other(int x){ //已知一个顶点获取另外一个顶点
        return s == m_s ? m_d : m_s;
    }
 
    // 重载<<(输出)运算符
    friend ostream & operator<<(ostream & os, const Edge<Weight> & e){
        os << e.m_s << "-" << e.m_d << ": " << e.m_weight;
        return os;
    }

    // 重载比较运算符
    bool operator<(Edge<Weight> & e){
        return m_weight < e.getWeight();
    }

    bool operator<=(Edge<Weight> & e){
        return m_weight <= e.getWeight();
    }

    bool operator>(Edge<Weight> & e){
        return m_weight > e.getWeight();
    }

    bool operator>=(Edge<Weight> & e){
        return m_weight >= e.getWeight();
    }

    bool operator==(Edge<Weight> & e){
        return m_weight == e.getWeight();
    }

    bool operator!=(Edge<Weight> & e){
        return m_weight != e.getWeight();
    }
}; 

#endif