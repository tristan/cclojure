#include <tuple>
#include <sstream>
#include "clojure.h"

// TODO: learn more about the typeid function
// and type_info stuff, because it seems (at
// least pre-c++11) that this is a bad way
// to do this

// returns true if the Object is a type the repl
// can simply print
inline bool is_plain_type(const std::shared_ptr<const Object> &o) {
  return 
    o->instanceof(typeid(Integer)) ||
    o->instanceof(typeid(Irrational)) ||
    o->instanceof(typeid(Ratio)) ||
    o->instanceof(typeid(Boolean)) ||
    o->instanceof(typeid(Symbol)) ||
    o->instanceof(typeid(Keyword))
    ;
}

inline std::tuple<int,int> seq_type(const std::shared_ptr<const Object> &o) {
  if (o->instanceof(typeid(List))) {
    return std::tuple<int,int>{'(', ')'};
  } else if (o->instanceof(typeid(Vector))) {
    return std::tuple<int,int>{'[', ']'};
  }
  return std::tuple<int,int>{ 0, 0 }; 
}

// TODO: const <const> & ????????
// find out what all of this really means!
// should all of my shared_ptr's be to const Objects?
// this seems like what i'd want seeing as all objects should be immutable
void utils::print(const std::shared_ptr<const Object> &o, std::ostream &out) {
  if (o == nullptr) {
    out << "nil";
    return;
  }
  std::tuple<int,int> brackets = seq_type(o);
  if (std::get<0>(brackets) != 0) {
    const std::shared_ptr<const Seq> s = std::dynamic_pointer_cast<const Seq>(o);
    out << (char)std::get<0>(brackets);
    if (s->count() > 0) {
      print(s->first(), out);
    }
    if (s->count() > 1) {
      auto r = s->rest();
      do {
        out << " ";
        print(r->first(), out);
        r = r->rest();
      } while (r != nullptr);
    }
    out << (char)std::get<1>(brackets);
  } else if (o->instanceof(typeid(Map)) || o->instanceof(typeid(Set))) {
    // TODO: map/set->toString() needs to call this instead of the other way around
    // this should be done once i figure out the details of how to represent
    // the interface tree that java clojure uses in an easy and efficient way
    out << o->toString();
  } else if (o->instanceof(typeid(String))) {
    out << "\"" << *o << "\"";
  } else if (o->instanceof(typeid(Pattern))) {
    out << "#\"" << o->toString() << "\"";
  } else if (o->instanceof(typeid(Character))) {
    out << "\\";
    std::string s = o->toString();
    if (s[0] == '\n') {
      out << "newline";
    } else if (s[0] == ' ') {
      out << "space";
    } else if (s[0] == '\t') {
      out << "tab";
    } else if (s[0] == '\b') {
      out << "backspace";
    } else if (s[0] == '\f') {
      out << "formfeed";
    } else if (s[0] == '\r') {
      out << "return";
    } else {
      out << s;
    }
  } else if (is_plain_type(o)) {
    out << *o;
  } else {
    out << "#<" << typeid(*o).name() << ": " << *o << ">";
  }
  //out << " (" << std::hex << o << ")";
}

std::string utils::print_string(const std::shared_ptr<const Object> &o) {
  std::stringstream ss;
  print(o, ss);
  return ss.str();
}
