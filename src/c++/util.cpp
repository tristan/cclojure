#include <list>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <functional>
#include "clojure.h"
#include "util.h"

using namespace clojure;

template <typename T>
bool boost_any_equals(object const &lhs, object const &rhs) {
  return boost::any_cast<T>(*lhs) == boost::any_cast<T>(*rhs);
}

bool util::equals(object const &lhs, object const &rhs) {
  if (lhs->type() != rhs->type()) {
    return false;
  }
  if (lhs->type() == typeid(std::nullptr_t)) {
    return true;
  }
  if (lhs->type() == typeid(bool)) {
    return boost_any_equals<bool>(lhs, rhs);
  }
  if (lhs->type() == typeid(int)) {
    return boost_any_equals<int>(lhs, rhs);    
  }
  if (lhs->type() == typeid(double)) {
    return boost_any_equals<double>(lhs, rhs);    
  }
  if (lhs->type() == typeid(ratio)) {
    ratio l = boost::any_cast<ratio>(*lhs);
    ratio r = boost::any_cast<ratio>(*rhs);
    return l.num ^ l.den == r.num ^ r.den;
  }
  if (lhs->type() == typeid(string)) {
    return boost_any_equals<string>(lhs, rhs);
  }
  if (lhs->type() == typeid(map)) {
    return boost_any_equals<map>(lhs, rhs);
  }
  if (lhs->type() == typeid(set)) {
    return boost_any_equals<set>(lhs, rhs);
  }
  if (lhs->type() == typeid(list)) {
    return boost_any_equals<list>(lhs, rhs);
  }
  if (lhs->type() == typeid(vector)) {
    return boost_any_equals<vector>(lhs, rhs);
  }
  return util::hash(lhs) == util::hash(rhs);
}

int double_hash(double val) {
  // taken from openjdk's Double.hashCode implementation
  union {
    long l;
    double d;
  } u;
  u.d = val;
  
  long bits = u.l;
  if ( ((bits & 0x7FF0000000000000L) ==
        0x7FF0000000000000L) &&
       (bits & 0x000FFFFFFFFFFFFFL) != 0L) {
    bits = 0x7ff8000000000000L;
  }
  return (int)(bits ^ (bits >> 32));
}

size_t hash_combine(size_t seed, size_t hash) {
  return seed ^ hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

size_t util::hash(object const &o) {
  if (o->type() == typeid(std::nullptr_t)) {
    return 0;
  }
  if (o->type() == typeid(symbol)) {
    symbol sym = boost::any_cast<symbol>(*o);
    return hash_combine(std::hash<string>()(sym.name),
                        sym.ns == "" ? 0 : std::hash<string>()(sym.ns));
  }
  if (o->type() == typeid(keyword)) {
    keyword kwd = boost::any_cast<keyword>(*o);
    return hash_combine(std::hash<string>()(kwd.sym.name),
                        kwd.sym.ns == "" ? 0 : std::hash<string>()(kwd.sym.ns))
      + 0x9e3779b9;
  }
  if (o->type() == typeid(int)) {
    return boost::any_cast<int>(*o);
  }
  if (o->type() == typeid(double)) {
    return double_hash(boost::any_cast<double>(*o));
  }
  if (o->type() == typeid(ratio)) {
    ratio m = boost::any_cast<ratio>(*o);
    return m.num ^ m.den;
  }
  if (o->type() == typeid(string)) {
    return std::hash<string>()(boost::any_cast<string>(*o));
  }
  if (o->type() == typeid(map)) {
    size_t hash = 0;
    map m = boost::any_cast<map>(*o);
    for (auto it = m.begin(); it != m.end(); it++) {
      auto e = *it;
      hash += (util::hash(e.first) ^ util::hash(e.second));
    }
    return hash;
  }
  if (o->type() == typeid(set)) {
    size_t hash = 0;
    set m = boost::any_cast<set>(*o);
    for (auto it = m.begin(); it != m.end(); it++) {
      auto e = *it;
      hash += util::hash(e);
    }
    return hash;
  }
  if (o->type() == typeid(list)) {
    size_t hash = 0;
    list m = boost::any_cast<list>(*o);
    for (auto it = m.begin(); it != m.end(); it++) {
      auto e = *it;
      hash = 31 * hash + util::hash(e);
    }
    return hash;
  }
  if (o->type() == typeid(vector)) {
    size_t hash = 0;
    vector m = boost::any_cast<vector>(*o);
    for (auto it = m.begin(); it != m.end(); it++) {
      auto e = *it;
      hash = 31 * hash + util::hash(e);
    }
    return hash;
  }
  return (size_t)o.get();
}
