#ifndef RANGE_H_
#define RANGE_H_

#include <stdint.h>

class Range{
public:
    Range(int low, int up) : 
        m_lowerBound(low),
        m_upperBound(up){}

    // 采用默认的析构、拷贝构造、赋值运算符函数

    inline uint32_t get_lowerBound() const {
        return m_lowerBound; 
    }

    inline uint32_t get_upperBound() const {
        return m_upperBound;
    }

    bool operator<(const Range & other) const {
        return ((m_lowerBound < other.get_lowerBound()) 
            || (!(other.get_lowerBound() < m_lowerBound) && m_upperBound < other.get_upperBound()));
    }

    bool can_merge(const Range & other){
        return !((other.get_lowerBound() > m_upperBound + 1) || (other.get_upperBound() < m_lowerBound -1));
    }

    Range merge(const Range & other) const {
        uint32_t lowerBound = m_lowerBound <= other.get_lowerBound() ? m_lowerBound : other.get_lowerBound();
        uint32_t upperBound = m_upperBound >= other.get_upperBound() ? m_upperBound : other.get_upperBound();

        return Range(lowerBound, upperBound);
    }

private:
    uint32_t m_lowerBound;
    uint32_t m_upperBound;
};

#endif // RANGE_H_