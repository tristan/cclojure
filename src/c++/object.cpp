#include <iostream>
#include "clojure.h"

std::shared_ptr<object> object::nil = nullptr;
std::shared_ptr<object> object::T = std::make_shared<boolean>(true);
std::shared_ptr<object> object::F = std::make_shared<boolean>(false);

std::ostream& operator<<(std::ostream& out, std::shared_ptr<object> o) {
  if (o == nullptr) {
    out << "nil";
  } else {
    out << o->to_string();
  }
  return out;
}

std::string object::to_string() {
  return "#<unknown object>";
}

bool object::operator==(const object &o) const {
  // objects themselves are only considered equal if they have the same address
  std::cout << "(object==) ";
  return (this == &o);
}

string::string(std::string s) {
  this->str = s;
}

std::string string::to_string() {
  return "\"" + this->str + "\"";
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

boolean::boolean(bool b) {
  this->value = b;
}

std::string boolean::to_string() {
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

bool operator==(std::shared_ptr<object> o1, std::shared_ptr<object> o2) {
  std::cout << "(obj==obj) ";
  if (o1 == nullptr && o2 == nullptr) {
    return true;
  } else if (o1 == nullptr || o2 == nullptr) {
    return false;
  } else if (o1.get() == o2.get()) {
    return true;
  }
  return *o1 == *o2;
}
