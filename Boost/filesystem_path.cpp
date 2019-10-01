#include <iostream>
#include <string>

#include <stdlib.h>

#include <boost/filesystem.hpp>

using namespace boost::filesystem;

int main(){
    path p(::getenv("PWD"));
    std::cout << p.string() << std::endl;
    std::cout << p.parent_path().string() << std::endl;
    std::cout << p.filename().string() << std::endl;

    return 0;
}