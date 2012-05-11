#include "clojure.h"

#include <sstream>
#include <iostream>

Map::Map() : _meta(nullptr) {}

Map::Map(const std::list<std::shared_ptr<Object> > &init) : _meta(nullptr) {
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

Map::Map(std::shared_ptr<Map> meta, 
         const std::map<std::shared_ptr<Object>,std::shared_ptr<Object>,compare_hashcodes> &map) 
  : _meta(meta), map(map) {}

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

std::shared_ptr<Map> Map::assoc(std::shared_ptr<Object> key, std::shared_ptr<Object> val) const {
  std::shared_ptr<Map> rval = std::make_shared<Map>( this->_meta, map );
  rval->map.insert(std::make_pair(key, val));
  return rval;
}

bool Map::instanceof(const std::type_info &info) const {
  return (
          typeid(Map) == info ||
          typeid(Meta) == info ||
          typeid(Iterable) == info
          );
}

using map_intern_type = std::map<std::shared_ptr<Object>,std::shared_ptr<Object>,compare_hashcodes>;

std::shared_ptr<Map> Map::meta() const {
  return _meta;
}

std::shared_ptr<Object> Map::withMeta(std::shared_ptr<Map> meta) const {
  return std::make_shared<Map>(meta, map);
}

Map::iterator::iterator(map_intern_type::const_iterator it) : it(it) {}

std::pair<const std::shared_ptr<Object>,std::shared_ptr<Object> > Map::iterator::operator*() {
  return *it;
}
  
Map::iterator& Map::iterator::operator++() {
  ++it;
  return *this;
}

bool Map::iterator::operator==(const Map::iterator& rhs) {
  return it == rhs.it;
}
 
bool Map::iterator::operator!=(const Map::iterator& rhs) {
  return it != rhs.it;
}

Map::iterator Map::begin() const {
  return Map::iterator(map.begin());
}
Map::iterator Map::end() const {
  return Map::iterator(map.end());
}

std::pair<std::shared_ptr<Object>,std::shared_ptr<Object> > Map::entryAt(std::shared_ptr<Object> key) {
  auto r = map.find(key);
  if (r != map.end()) {
    return *r;
  }
  return std::make_pair(nullptr, nullptr);
}

std::shared_ptr<Object> Map::valAt(std::shared_ptr<Object> key) {
  auto r = map.find(key);
  if (r != map.end()) {
    return r->second;
  }
  return nullptr;
}
