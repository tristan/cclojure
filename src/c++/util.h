#ifndef _CLJ_UTIL_H
#define _CLJ_UTIL_H

namespace util {
  size_t hash(clojure::object const &o);
  int compare(clojure::object const &lhs, clojure::object const &rhs);
  bool equals(clojure::object const &lhs, clojure::object const &rhs);
}

#endif
