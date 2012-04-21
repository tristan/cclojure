#include "clojure.h"

boolean::boolean(bool b) {
  this->value = b;
}

std::string boolean::to_string() const {
  return this->value ? "true" : "false";
}

bool boolean::operator==(const object &o) const {
  try {
    const boolean &b = dynamic_cast<const boolean&>(o);
    return this->value == b.value;
  } catch (std::bad_cast &e) {
    // TODO: same as string::operator==
    return false;
  }
}
