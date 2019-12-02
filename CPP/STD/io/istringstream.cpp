#include <sstream>
#include <iostream>

int main(){
  std::string str("liuyunian 23 jinan");
  std::istringstream iss(str);
  std::cout << "str = " << iss.str() << "\n";
  std::string name;
  int age;
  std::string home;

  iss >> name >> age >> home;
  
  std::cout << "name = " << name << '\n';
  std::cout << "age = " << age << '\n';
  std::cout << "home = " << home << std::endl;

  return 0;
}