#include <iostream>
#include <any>

class Test {
public:
  Test(){}

private:
  int m_data;
  std::string m_name;
};

void test_func(const std::any &t){
  std::cout << t.type().name() << std::endl;
}

int main(){
  std::cout << std::boolalpha;
 
  std::any a = 1;
  std::cout << a.type().name() << ": " << std::any_cast<int>(a) << '\n';

  a = 3.14;
  std::cout << a.type().name() << ": " << std::any_cast<double>(a) << '\n';

  a = true;
  std::cout << a.type().name() << ": " << std::any_cast<bool>(a) << '\n';

  // a = Test();
  // std::cout << a.type().name() << std::endl;
  test_func(Test());

  return 0;
}