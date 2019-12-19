/**
 * test random: default_random_engine
*/

#include <iostream>
#include <random>

int main(){
  std::default_random_engine e;
  for(int i = 10; i > 0; -- i){
    std::cout << e() << std::endl;
  }

  return 0;
}