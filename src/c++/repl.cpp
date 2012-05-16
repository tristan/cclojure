#include <iostream>
#include <string>
#include <cstddef>
#include "clojure.h"
#include "repl.h"
#include "lispreader.h"
#include "compiler.h"
#include "runtime.h"

int cpp_repl(std::istream &in) {
  auto ns = "user"; //Namespace::findOrCreate("user");
  auto eof_value = clojure::make_object(nullptr);
  std::cout << "Clojure c++ 1.0a" << std::endl;
  while (1) {
    std::cout << ns << "=> ";
    try {
      // read
      auto o = clojure::read(in, false, eof_value, false);
      // eval
      if (o == eof_value) {
        std::cout << std::endl;
        return 0;
      }
      // TODO: eval before print (Compiler.java:5389)
      o = compiler::eval(o);
      // print
      runtime::print(o, std::cout);
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
