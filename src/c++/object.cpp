#include <iostream>
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

std::shared_ptr<object> object::nil = std::make_shared<nil_class>();

std::ostream& operator<<(std::ostream& out, std::shared_ptr<object> o) {
  out << o.get()->to_string();
  return out;
}

std::string object::to_string() {
  return "#<" + this->java_type + ">";
}

bool object::operator==(const object &o2) {
  // objects themselves are only considered equal if they have the same address
  return (this == &o2);
}
