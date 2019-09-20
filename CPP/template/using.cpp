#include <iostream>
#include <map>

// typedef unsigned int u_i;
// using u_i = unsigned int;

// typedef int (*Func)(int, int);
// using Func = int (*)(int, int);

// typedef std::map<std::string, int> map_s_i;
// using map_s_i = std::map<std::string, int>;

// void add(u_i a, u_i b){
//     std::cout << a+b << std::endl;
// }

// template<typename T>
// using map_s = std::map<std::string, T>;

template <typename T>
using Func = int(*)(T, T);

int test(int i, int j){
    return i+j;
}

int main(){
    // u_i a = 9;
    // std::cout << a << std::endl;

    // add(1, 2);
    // map_s<int> m;
    // m.insert({"first", 10});
    Func<int> func = test;
    std::cout << func(1, 2) << std::endl;

    return 0;
}

