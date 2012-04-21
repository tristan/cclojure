#include <iostream>
#include "clojure.h"

std::shared_ptr<Object> Object::nil = nullptr;
std::shared_ptr<Object> Object::T = std::make_shared<Boolean>(true);
std::shared_ptr<Object> Object::F = std::make_shared<Boolean>(false);

std::ostream& operator<<(std::ostream& out, const Object& o) {
  out << o.toString();
  return out;
}

bool Object::operator==(const Object &o) const {
  // Objects themselves are only considered equal if they have the same address
  std::cout << "(Object==) ";
  return (this == &o);
}
