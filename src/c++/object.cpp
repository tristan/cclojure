#include <iostream>
#include "clojure.h"

std::shared_ptr<object> object::nil = nullptr;
std::shared_ptr<object> object::T = std::make_shared<boolean>(true);
std::shared_ptr<object> object::F = std::make_shared<boolean>(false);

std::ostream& operator<<(std::ostream& out, const object& o) {
  out << o.to_string();
  return out;
}

bool object::operator==(const object &o) const {
  // objects themselves are only considered equal if they have the same address
  std::cout << "(object==) ";
  return (this == &o);
}
