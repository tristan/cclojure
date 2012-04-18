#include <iostream>
#include <string>
#include <cstddef>
#include "clojure.h"
#include "repl.h"
#include "lispreader.h"

int cpp_repl(std::istream &in) {
  std::string ns = "user";
  lispreader reader;
  obj eof_value = std::make_shared<integer>(1L);
  while (1) {
    std::cout << ns << "=> ";
    try {
      auto o = reader.read(in, false, eof_value, false);
      std::cout << o << std::endl;
      if (o == eof_value) {
        return 0;
      }
    } catch (std::string s) {
      std::cout << s << std::endl;
    } catch (const char *s) {
      std::cout << s << std::endl;
    }
  }
  std::cout << std::endl;
  return 0;
}
