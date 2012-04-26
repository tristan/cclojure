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
inline bool is_string_type(const std::shared_ptr<const Object> &o) {
  return typeid(*o) == typeid(String);
}

inline bool is_char_type(const std::shared_ptr<const Object> &o) {
  return typeid(*o) == typeid(Character);
}

inline bool is_map_type(const std::shared_ptr<const Object> &o) {
  return typeid(*o) == typeid(Map);
}

inline bool is_set_type(const std::shared_ptr<const Object> &o) {
  return typeid(*o) == typeid(Set);
}

inline bool is_regex_pattern_type(const std::shared_ptr<const Object> &o) {
  return typeid(*o) == typeid(Pattern);
}

// returns true if the Object is a type the repl
// can simply print
inline bool is_plain_type(const std::shared_ptr<const Object> &o) {
  return 
    typeid(*o) == typeid(Integer) ||
    typeid(*o) == typeid(Irrational) ||
    typeid(*o) == typeid(Ratio) ||
    typeid(*o) == typeid(Boolean) ||
    typeid(*o) == typeid(Symbol) ||
    typeid(*o) == typeid(Keyword)
    ;
}

inline std::tuple<int,int> seq_type(const std::shared_ptr<const Object> &o) {
  if (typeid(*o) == typeid(List)) {
    return std::tuple<int,int>{'(', ')'};
  } else if (typeid(*o) == typeid(Vector)) {
    return std::tuple<int,int>{'[', ']'};
  }
  return std::tuple<int,int>{ 0, 0 }; 
}

// returns whether or not the object supports metadata (i.e. implements IMeta)
// TODO: note that this is very hard-coded, which may be ok since we don't
// really expect any new class types as of yet, but wont allow for easy
// expansion
bool utils::supports_meta(const std::shared_ptr<const Object> &o) {
  return 
    typeid(*o) == typeid(Vector) ||
    typeid(*o) == typeid(Map) ||
    typeid(*o) == typeid(Set) ||
    typeid(*o) == typeid(List) ||
    typeid(*o) == typeid(Symbol)
    ;
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
      } while (r != Object::nil);
    }
    out << (char)std::get<1>(brackets);
  } else if (is_map_type(o) || is_set_type(o)) {
    // TODO: map/set->toString() needs to call this instead of the other way around
    // this should be done once i figure out the details of how to represent
    // the interface tree that java clojure uses in an easy and efficient way
    out << o->toString();
  } else if (is_string_type(o)) {
    out << "\"" << *o << "\"";
  } else if (is_regex_pattern_type(o)) {
    out << "#\"" << o->toString() << "\"";
  } else if (is_char_type(o)) {
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
