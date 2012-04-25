#include <iostream>
#include <string>
#include <cstddef>
#include "clojure.h"
#include "repl.h"
#include "lispreader.h"


int cpp_repl(std::istream &in) {
  auto ns = Namespace::findOrCreate("user");
  std::shared_ptr<Object> eof_value = std::make_shared<Integer>(-1L);
  std::cout << "Clojure c++ 1.0a" << std::endl;
  while (1) {
    std::cout << ns->toString() << "=> ";
    try {
      // read
      auto o = LispReader::read(in, false, eof_value, false);
      // eval
      if (o == eof_value) {
        std::cout << std::endl;
        return 0;
      }
      // TODO: eval before print (Compiler.java:5389)
      // print
      utils::print(o, std::cout);
      std::cout << std::endl;
    } catch (std::string s) {
      std::cout << s << std::endl;
    } catch (const char *s) {
      std::cout << s << std::endl;
    }
  }
  std::cout << std::endl;
  return 0;
}
