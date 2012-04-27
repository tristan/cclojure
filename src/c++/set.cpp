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

Set::Set(std::shared_ptr<Map> meta, 
         const std::set<std::shared_ptr<Object>,compare_hashcodes> &set)
  : _meta(meta), set(set) {}

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

bool Set::instanceof(const std::type_info &info) const {
  return (
          typeid(Set) == info ||
          typeid(Meta) == info
          );
}

std::shared_ptr<Map> Set::meta() const {
  return _meta;
}

std::shared_ptr<Object> Set::withMeta(std::shared_ptr<Map> meta) const {
  return std::make_shared<Set>(meta, set);
}
