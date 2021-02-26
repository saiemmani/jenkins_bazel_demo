#include "lib/service.h"
#include "nlohmann/json.hpp"
#include <iostream>

int main(int argc, char **argv) {
  nlohmann::json obj = {
      {"bazel", "https://bazel.build"},
      {"cmake", "https://cmake.org/"},
  };
  std::cout << obj.dump(4) << std::endl;
  std::cout << greet("Maroun") << std::endl;
}
