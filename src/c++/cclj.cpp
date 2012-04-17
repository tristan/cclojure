#include <iostream>
#include <fstream>
#include "repl.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    return cpp_repl(std::cin);
  } else {
    std::ifstream f { argv[1] };
    return cpp_repl(f);
  }
  return 0;
}
