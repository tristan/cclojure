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

// returns true if the object is a string. used
// so the repl can wrap it in double quotes when
// printing the value
inline bool is_string_type(std::shared_ptr<object> o) {
  return typeid(*o) == typeid(string);
}

// returns true if the object is a type the repl
// can simply print
inline bool is_plain_type(std::shared_ptr<object> o) {
  return 
    typeid(*o) == typeid(integer) ||
    typeid(*o) == typeid(irrational) ||
    typeid(*o) == typeid(ratio) ||
    typeid(*o) == typeid(boolean) ||
    typeid(*o) == typeid(symbol) ||
    typeid(*o) == typeid(keyword)
    ;
}

int cpp_repl(std::istream &in) {
  auto ns = Namespace::findOrCreate(symbol::create( "user" ));
  lispreader reader;
  std::shared_ptr<object> eof_value = std::make_shared<integer>(-1L);
  std::cout << "Clojure c++ 1.0a" << std::endl;
  while (1) {
    std::cout << *ns->get_name() << "=> ";
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
      if (o == object::nil) {
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
