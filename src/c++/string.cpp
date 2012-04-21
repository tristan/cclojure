#include <functional>
#include "clojure.h"

string::string(const std::string &s) {
  // TODO: does this copy? if it compiles i'm assuming yes!
  this->str = std::string{ s }; 
}

string::string(const string &s) {
  // TODO: does this copy? if it compiles i'm assuming yes!
  this->str = std::string{ s.str }; 
}

std::string string::to_string() const {
  // removing the double-quotes here because it should be
  // the repl's job to know it's printing a string and
  // add the quotes itself
  return str;
}

size_t string::hash_code() const {
  return std::hash<std::string>()(str);
}

bool string::operator==(const object &o) const {
  try {
    const string &s = dynamic_cast<const string&>(o);
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

