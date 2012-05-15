#include "clojure.h"

namespace clojure {
  object NIL = make_object(nullptr);
  object TRUE = make_object(true);
  object FALSE = make_object(false);
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
