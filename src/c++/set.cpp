#include "clojure.h"

#include <sstream>
#include <iostream>

Set::Set() {}

Set::Set(const std::list<std::shared_ptr<Object> > &init) {
  auto i = init.begin();
  while (i != init.end()) {
    std::shared_ptr<Object> val = *i++;
    auto r = set.insert(val);
    if (r.second == false) {
      throw "Duplicate key: " + val->toString();
    }
  }
}

size_t Set::count() const {
  return set.size();
}

std::string Set::toString() const {
  std::stringstream out;
  out << "#{";
  for (auto i = set.begin(); i != set.end(); ) {
    auto entry = *i++;
    utils::print(entry, out);
    if (i != set.end()) {
      out << " ";
    }
  }
  out << "}";
  return out.str(); //utils::print_string( shared_from_this() );
}
