#include <iostream>
#include <string>
#include "clojure.h"

class nil_class : public object {
public:
  nil_class() {
    this->java_type = "null";
  }
  std::string to_string() override {
    return "nil";
  }
};

object object::nil = nil_class {};

std::ostream& operator<<(std::ostream& out, object& o) {
  out << o.to_string();
  return out;
}

std::string object::to_string() {
  return "#<" + this->java_type + ">";
}

bool object::operator==(const object &o2) {
  // objects themselves are only considered equal if they have the same address
  return (this == &o2);
}
