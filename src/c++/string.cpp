#include <functional>
#include "clojure.h"

String::String(const std::string &s) : str(s) {}

String::String(const String &s) : str(s.str) {}

std::string String::toString() const {
  // removing the double-quotes here because it should be
  // the repl's job to know it's printing a String and
  // add the quotes itself
  return str;
}

size_t String::hashCode() const {
  return std::hash<std::string>()(str);
}

bool String::operator==(const Object &o) const {
  try {
    const String &s = dynamic_cast<const String&>(o);
    return this->str == s.str;
  } catch (std::bad_cast &e) {
    // TODO: i don't like the idea of using try catch
    // as part of non-error handling program flow
    // I need to read up more on this in c++ to see
    // if it's really ok, or if i could find
    // some other way to cast things
    return false;
  }
}

