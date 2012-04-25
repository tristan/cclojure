#include "clojure.h"

Boolean::Boolean(bool b) : value(b) {}

std::string Boolean::toString() const {
  return this->value ? "true" : "false";
}

bool Boolean::operator==(const Object &o) const {
  try {
    const Boolean &b = dynamic_cast<const Boolean&>(o);
    return this->value == b.value;
  } catch (std::bad_cast &e) {
    // TODO: same as string::operator==
    return false;
  }
}

bool Boolean::instanceof(const std::type_info &info) const {
  return (
          typeid(Boolean) == info
          );
}
