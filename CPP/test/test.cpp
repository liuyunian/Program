#include <iostream>
#include <boost/type_index.hpp>

// void func(int data){
//     std::cout << data << std::endl;
// }

int func(int data){
    std::cout << data << std::endl;
}

int main(){
    // auto i = 3;
    // const auto j = 4;
    // int i = 9;
    // const int & j = i;
    // auto x = j;
    // int data  = 10;
    // auto & x = data;
    // const auto & y = data; 
    // auto & i = y;

    // auto * x = &data;
    // auto y = &data;

    // auto p = new int(10);

    // auto && ref = 10;
    // int data = 1;
    // auto && ref2 = data;
    // auto && ref3 = std::move(data);

    // int data[] = {1, 2};
    // auto i = data;
    // auto & j = data;

    // const char str[] = "hello";
    // auto m = str;
    // auto & n = str;

    // auto f = func;

    // int i = 10;
    // decltype(i) data; // data = int

    // const int j = 10;
    // decltype(j) data = 0;

    // int &j = i;
    // decltype(j) data = j;

    // const int &j = i;
    // decltype(j) data = i;

    // int * p = &i;
    // decltype(p) data;

    // int && j = std::move(i);
    // decltype(j) data = 10;

    // int i = 10;
    // int * p = &i;
    // decltype(*p) data = i;

    // decltype(func) data;

    using boost::typeindex::type_id_with_cvr;
    std::cout << "data = " << type_id_with_cvr<decltype(data)>().pretty_name() << std::endl;

    return 0;
}