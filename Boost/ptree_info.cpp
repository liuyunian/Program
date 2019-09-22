#include <iostream>
#include <string.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

using namespace boost::property_tree;

int main(){
    ptree pt;
    read_info("./ptree_info.conf", pt);

    std::cout << pt.get<std::string>("test.ip_addr") << std::endl;
    std::cout << pt.get<int>("test.port") << std::endl;

    return 0;
}