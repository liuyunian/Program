#include <iostream>
#include <set>

#include "Range.h"

int main(){
    /**
     * 排序
    std::set<Range> m_testSet;
    for(int i = 10; i > 0; -- i){
        m_testSet.insert(Range(i, i));
    }

    for(auto & item : m_testSet){
        std::cout << item.get_lowerBound() << ", " << item.get_upperBound() << std::endl;
    }*/

    Range r(100, 200);

    Range r2(1, 50);
    Range r3(1, 99);
    Range r4(1, 150);
    Range r5(1, 200);
    Range r6(1, 250);

    Range r7(100, 300);
    Range r8(101, 300);
    Range r9(200, 300);
    Range r10(201, 300);
    Range r11(250, 300);

    std::cout << std::noboolalpha;
    std::cout << r.can_merge(r2) << std::endl;
    std::cout << r.can_merge(r3) << std::endl;
    std::cout << r.can_merge(r4) << std::endl;
    std::cout << r.can_merge(r5) << std::endl;
    std::cout << r.can_merge(r6) << std::endl;
    std::cout << r.can_merge(r7) << std::endl;
    std::cout << r.can_merge(r8) << std::endl;
    std::cout << r.can_merge(r9) << std::endl;
    std::cout << r.can_merge(r10) << std::endl;
    std::cout << r.can_merge(r11) << std::endl;

    const Range & r_merge = r.merge(r3);
    std::cout << r_merge.get_lowerBound() << ", " << r_merge.get_upperBound() << std::endl;

    const Range & r_merge2 = r.merge(r4);
    std::cout << r_merge2.get_lowerBound() << ", " << r_merge2.get_upperBound() << std::endl;

    const Range & r_merge3 = r_merge.merge(r8);
    std::cout << r_merge3.get_lowerBound() << ", " << r_merge3.get_upperBound() << std::endl;

    return 0;
}