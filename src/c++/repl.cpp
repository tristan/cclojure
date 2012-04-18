#include <iostream>
#include <string>
#include <cstddef>
#include "clojure.h"
#include "repl.h"
#include "lispreader.h"

int cpp_repl(std::istream &in) {
  std::string ns = "user";
  lispreader reader;
  object& eof_value = object::nil;
  while (1) {
    std::cout << ns << "=> ";
    object &o = reader.read(in, false, eof_value, false);
    std::cout << std::endl << o << std::endl;
    if (o == eof_value) {
      return 0;
    } else {
      return 0;
    }
  }
  std::cout << std::endl;
  return 0;
}
