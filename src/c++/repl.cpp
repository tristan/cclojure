#include <iostream>
#include <string>
#include <cstddef>
#include "clojure.h"
#include "repl.h"
#include "lispreader.h"

int cpp_repl(std::istream &in) {
  auto ns = Namespace::findOrCreate(symbol::create( "user" ));
  lispreader reader;
  std::shared_ptr<object> eof_value = std::make_shared<integer>(-1L);
  std::cout << "Clojure c++ 1.0a" << std::endl;
  while (1) {
    std::cout << ns->get_name() << "=> ";
    try {
      auto o = reader.read(in, false, eof_value, false);
      // TODO: here, i probably want to compare pointers, maybe obj==obj is a bad idea
      if (o == eof_value) {
        std::cout << std::endl;
        return 0;
      }
      // TODO: eval before print (Compiler.java:5389)
      std::cout << o << " (" << std::hex << o.get() << ")" << std::endl;
    } catch (std::string s) {
      std::cout << s << std::endl;
    } catch (const char *s) {
      std::cout << s << std::endl;
    }
  }
  std::cout << std::endl;
  return 0;
}
