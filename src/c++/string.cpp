#include <functional>
#include "clojure.h"

String::String(const std::string &s) : str(s) {}

String::String(const String &s) : str(s.str) {}

std::string String::toString() const {
  return str;
}

size_t String::hashCode() const {
  return std::hash<std::string>()(str);
}

bool String::operator==(const Object &o) const {
  if (typeid(o) != typeid(String)) {
    return false;
  }
  const String &s = dynamic_cast<const String&>(o);
  return this->str == s.str;
}

int String::compareTo(const Comparable &o) const {
  if (typeid(o) != typeid(String)) {
    throw std::string(typeid(o).name()) + " cannot be cast to String";
  }
  return this->str.compare( dynamic_cast<const String &>(o).str );
}

bool String::instanceof(const std::type_info &info) const {
  return (
          typeid(String) == info ||
          typeid(Comparable) == info
          );
}
