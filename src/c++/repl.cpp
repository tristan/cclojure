#include <iostream>
#include <string>
#include <cstddef>
#include "clojure.h"
#include "repl.h"
#include "lispreader.h"

// TODO: learn more about the typeid function
// and type_info stuff, because it seems (at
// least pre-c++11) that this is a bad way
// to do this

// returns true if the Object is a string. used
// so the repl can wrap it in double quotes when
// printing the value
inline bool is_string_type(std::shared_ptr<Object> o) {
  return typeid(*o) == typeid(String);
}

// returns true if the Object is a type the repl
// can simply print
inline bool is_plain_type(std::shared_ptr<Object> o) {
  return 
    typeid(*o) == typeid(Integer) ||
    typeid(*o) == typeid(Irrational) ||
    typeid(*o) == typeid(Ratio) ||
    typeid(*o) == typeid(Boolean) ||
    typeid(*o) == typeid(Symbol) ||
    typeid(*o) == typeid(Keyword)
    ;
}

int cpp_repl(std::istream &in) {
  auto ns = Namespace::findOrCreate("user");
  LispReader reader;
  std::shared_ptr<Object> eof_value = std::make_shared<Integer>(-1L);
  std::cout << "Clojure c++ 1.0a" << std::endl;
  while (1) {
    std::cout << ns->getName() << "=> ";
    try {
      // read
      auto o = reader.read(in, false, eof_value, false);
      // eval
      if (o == eof_value) {
        std::cout << std::endl;
        return 0;
      }
      // TODO: eval before print (Compiler.java:5389)
      // print
      if (o == Object::nil) {
        std::cout << "nil";
      } else if (is_string_type(o)) {
        std::cout << "\"" << *o << "\"";
      } else if (is_plain_type(o)) {
        std::cout << *o;
      } else {
        std::cout << "#<" << typeid(*o).name() << ": " << *o << ">";
      }
      std::cout << " (" << std::hex << o.get() << ")" << std::endl;
    } catch (std::string s) {
      std::cout << s << std::endl;
    } catch (const char *s) {
      std::cout << s << std::endl;
    }
  }
  std::cout << std::endl;
  return 0;
}
