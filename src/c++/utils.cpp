#include <tuple>
#include <sstream>
#include "clojure.h"

// TODO: learn more about the typeid function
// and type_info stuff, because it seems (at
// least pre-c++11) that this is a bad way
// to do this

// returns true if the Object is a string. used
// so the repl can wrap it in double quotes when
// printing the value
inline bool is_string_type(const Object* o) {
  return typeid(*o) == typeid(String);
}

// returns true if the Object is a type the repl
// can simply print
inline bool is_plain_type(const Object* o) {
  return 
    typeid(*o) == typeid(Integer) ||
    typeid(*o) == typeid(Irrational) ||
    typeid(*o) == typeid(Ratio) ||
    typeid(*o) == typeid(Boolean) ||
    typeid(*o) == typeid(Symbol) ||
    typeid(*o) == typeid(Keyword)
    ;
}

inline std::tuple<int,int> seq_type(const Object* o) {
  if (typeid(*o) == typeid(List)) {
    return std::tuple<int,int>{'(', ')'};
  }
  return std::tuple<int,int>{ 0, 0 }; 
}

void utils::print(const std::shared_ptr<Object> &o, std::ostream &out) {
  print(o.get(), out);
}

void utils::print(const Object *o, std::ostream &out) {
  if (o == 0) {
    out << "nil";
    return;
  }
  std::tuple<int,int> brackets = seq_type(o);
  if (std::get<0>(brackets) != 0) {
    const Seq *s = dynamic_cast<const Seq*>(o);
    out << (char)std::get<0>(brackets);
    if (s->count() > 0) {
      print(s->first().get(), out);
    }
    if (s->count() > 1) {
      auto r = s->rest();
      do {
        out << " ";
        print(r->first().get(), out);
        r = r->rest();
      } while (r != Object::nil);
    }
    out << (char)std::get<1>(brackets);
  } else if (is_string_type(o)) {
    out << "\"" << *o << "\"";
  } else if (is_plain_type(o)) {
    out << *o;
  } else {
    out << "#<" << typeid(*o).name() << ": " << *o << ">";
  }
  //out << " (" << std::hex << o << ")";
}

std::string utils::print_string(const std::shared_ptr<Object> &o) {
  return print_string(o.get());
}

std::string utils::print_string(const Object *o) {
  std::stringstream ss;
  print(o, ss);
  return ss.str();
}

/*

  std::stringstream ss;
  ss << "(";
  if (_count > 0) {
    ss << (_first == Object::nil ? "nil" : _first->toString());
    if (_rest != Object::nil) {
      ss << " ";
      std::shared_ptr<List> r = _rest;
      while (r->_count > 0) {
        ss << (r->_first == Object::nil ? "nil" : r->_first->toString());
        if (r->_rest == Object::nil) {
          break;
        }
        ss << " ";
        r = r->_rest;
      }
    }
  }
  ss << ")";
  return ss.str();


*/
