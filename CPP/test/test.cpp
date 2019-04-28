#include <iostream>
#include <boost/type_index.hpp>

void func(int data){
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

    auto f = func;

    using boost::typeindex::type_id_with_cvr;
    std::cout << "f = " << type_id_with_cvr<decltype(f)>().pretty_name() << std::endl;

    return 0;
}