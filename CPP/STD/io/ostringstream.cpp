#include <iostream>
#include <sstream>

int main(){
  std::ostringstream oss;

  std::string name("liuyunian");
  int age = 23;
  std::string home("jinan");
  oss << name << age << home;

  std::cout << oss.str() << std::endl;

  return 0;
}