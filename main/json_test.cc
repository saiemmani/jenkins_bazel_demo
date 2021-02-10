#include "nlohmann/json.hpp"
#include "lib/service.h"
#include <iostream>


#define JEAN "jean"
int main(int argc, char** argv) {
  nlohmann::json obj = {
      {"bazel", "https://bazel.build"},
      {"cmake", "https://cmake.org/"},    
  };
  std::cout << obj.dump(4) << std::endl;
  std::cout<< greet(argv[1]) << std::endl;

  std::cout << JEAN << std::endl;
}
