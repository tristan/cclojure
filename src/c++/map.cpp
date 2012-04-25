#include "clojure.h"

#include <sstream>
#include <iostream>

Map::Map() {}

Map::Map(const std::list<std::shared_ptr<Object> > &init) {
  auto i = init.begin();
  while (i != init.end()) {
    // shouldn't need to check that the values exist
    // as this is done by the lispreader
    std::shared_ptr<Object> key = *i++;
    std::shared_ptr<Object> val = *i++;
    auto r = map.insert(std::make_pair(key, val));
    if (r.second == false) {
      throw "Duplicate key: " + key->toString();
    }
  }
}

/* tried this, but i couldn't get it to work with make_shared
Map::Map(std::initializer_list<std::shared_ptr<Object> > init) {
  auto i = init.begin();
  while (i != init.end()) {
    // shouldn't need to check that the values exist
    // as this is done by the lispreader
    std::shared_ptr<Object> key = *i++;
    std::shared_ptr<Object> val = *i++;
    auto r = map.insert(std::make_pair(key, val));
    if (r.second == false) {
      throw "Duplicate key: " + key->toString();
    }
  }  
}
*/

size_t Map::count() const {
  return map.size();
}

std::string Map::toString() const {
  std::stringstream out;
  out << "{";
  for (auto i = map.begin(); i != map.end(); ) {
    auto entry = *i++;
    utils::print(entry.first, out);
    out << " ";
    utils::print(entry.second, out);
    if (i != map.end()) {
      out << ", ";
    }
  }
  out << "}";
  return out.str(); //utils::print_string( shared_from_this() );
}

bool Map::instanceof(const std::type_info &info) const {
  return (
          typeid(Map) == info
          );
}
