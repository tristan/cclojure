#include <iostream>
#include "clojure.h"
#include "runtime.h"
#include "util.h"

namespace clojure {
  object NIL = make_object(nullptr);
  object TRUE = make_object(true);
  object FALSE = make_object(false);
  std::map<object,object,objcomp> namespaces;
}

using namespace clojure;

std::string clojure::to_string(object const &o) {
  if (o->type() == typeid(std::nullptr_t)) {
    return "nil";
  }
  if (o->type() == typeid(bool)) {
    return boost::any_cast<bool>(*o) ? "true" : "false";
  }
  return "";
}

std::ostream &operator<<(std::ostream &out, object const &o) {
  out << to_string(o);
  return out;
}

bool clojure::objcomp::operator()(object const &lhs, object const &rhs) const {
  return util::hash(lhs) < util::hash(rhs);
}

object clojure::make_namespace(object const &sym) {
  if (sym->type() != typeid(symbol)) {
    throw "Error: cannot use non-symbol as namespace name";
  }
  auto it = namespaces.find(sym);
  if (it != namespaces.end()) {
    return it->second;
  }
  object ns = make_object( map() );
  namespaces[sym] = ns;
  return ns;
}

bool operator==(clojure::object const &lhs, clojure::object const &rhs) {
  if (lhs.get() == rhs.get()) {
    return true;
  }
  return util::equals(lhs, rhs);
}

bool operator!=(clojure::object const &lhs, clojure::object const &rhs) {
  if (lhs.get() == rhs.get()) {
    return false;
  }
  return !util::equals(lhs, rhs);
}
