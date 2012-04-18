#include <iostream>
#include "clojure.h"

obj object::nil = nullptr;

std::ostream& operator<<(std::ostream& out, obj o) {
  if (o == nullptr) {
    out << "nil";
  } else {
    out << o->to_string();
  }
  return out;
}

std::string object::to_string() {
  return "#<unknown object>";
}

bool object::operator==(const object &o) {
  // objects themselves are only considered equal if they have the same address
  std::cout << "object==\n";
  return (this == &o);
}

string::string(std::string s) {
  this->str = s;
}

std::string string::to_string() {
  return "\"" + this->str + "\"";
}

bool string::operator==(const object &o) {
  std::cout << "string==\n";
  const string &s = dynamic_cast<const string&>(o);
  return this->str == s.str;
}

bool operator==(obj o1, obj o2) {
  std::cout << "shared==\n";
  if (o1 == nullptr && o2 == nullptr) {
    return true;
  } else if (o1 == nullptr || o2 == nullptr) {
    return false;
  } else if (o1.get() == o2.get()) {
    return true;
  }
  return *o1 == *o2;
}
