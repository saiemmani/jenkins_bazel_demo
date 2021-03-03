#include "nlohmann/json.hpp"
#include "lib/service.h"
#include <iostream>


int main(int argc, char** argv) {
  nlohmann::json obj = {
      {"bazel", "https://bazel.build"},
      {"cmake", "https://cmake.org/"},    
  };
  std::cout << obj.dump(4) << std::endl;
  std::cout<< greet("Maroun") << std::endl;
  std::cout<< "Some extra source code by Maroun" << std::endl;
  std::cout<< "More source code by Maroun" << std::endl;
}
