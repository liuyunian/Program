/**
 * std::pair用法
*/

#include <iostream>
#include <string>
#include <map>

int main(){
    std::pair<int, float> p1;
    std::cout << p1.first << ", " << p1.second << std::endl;

    std::pair<int, std::string> p2(1, "liu");
    std::cout << p2.first << ", " << p2.second << std::endl;

    typedef std::pair<std::string, std::string> string_pair;
    string_pair sp3(string_pair("liu", "yunian"));

    std::cout << sp3.first << ", " << sp3.second << std::endl;

    std::map<int, std::string> map_test;
    map_test.insert(p2);
    map_test.insert({2, "yu"});
    map_test.insert(std::make_pair(3, "nian"));

    for(auto & item : map_test){
        std::cout << item.first << ", " << item.second << std::endl;
    }

    return 0;
}