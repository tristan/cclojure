#include <functional>
#include "clojure.h"

std::shared_ptr<symbol> symbol::create(std::string name) {
  return std::shared_ptr<symbol>( new symbol("", name) );
}

std::shared_ptr<symbol> symbol::create(std::string ns, std::string name) {
  return std::shared_ptr<symbol>( new symbol(ns, name) );
}

std::string symbol::get_name() {
  return name;
}

std::string symbol::to_string() {
  if (ns != "") {
    return ns + "/" + name;
  } else {
    return name;
  }
}

symbol::symbol(std::string ns, std::string name) {
  this->ns = ns;
  this->name = name;
  
  size_t name_hash = std::hash<std::string>()(name);
  size_t ns_hash = 0;
  if (ns != "") {
    ns_hash = std::hash<std::string>()(ns);
  }
  this->hash = ns_hash ^ name_hash + 0x9e3779b9 + (ns_hash << 6) + (ns_hash >> 2);
}

std::ostream& operator<<(std::ostream& out, std::shared_ptr<symbol> o) {
  if (o == nullptr) {
    out << object::nil;
  } else {
    out << o->get_name();
  }
  return out;
}
