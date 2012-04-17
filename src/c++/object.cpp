#include <string>
#include "clojure.h"

object::object(std::string symbol) {
  this->symbol = symbol;
}

std::ostream& operator<<(std::ostream& out, object& o) {
  out << o.symbol;
  return out;
}

bool object::operator==(const object &o2) {
  // objects themselves are only considered equal if they have the same address
  return (this == &o2);
}



object object::nil = object { "nil" };
