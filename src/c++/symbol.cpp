#include <functional>
#include "clojure.h"

std::shared_ptr<symbol> symbol::create(std::string name) {
  size_t slashpos = name.rfind("/");
  std::string ns = "";
  if (slashpos != std::string::npos) {
    ns = name.substr(0, slashpos-1);
    name = name.substr(slashpos+1);
  }
  return std::shared_ptr<symbol>( new symbol(ns, name) );
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

bool symbol::operator==(const object& o) {
  try {
    const symbol &s = dynamic_cast<const symbol&>(o);
    return this->name == s.name && this->ns == s.ns;
  } catch (std::bad_cast &e) {
    // TODO: same as string::operator==
    return false;
  }
}

int compare(const symbol& s1, const symbol& s2) {
  if (s1.ns == "" && s2.ns != "") {
    return -1;
  }
  if (s1.ns != "") {
    if (s2.ns == "") {
      return 1;
    }
    if (s1.ns < s2.ns) {
      return -1;
    }
    if (s1.ns > s2.ns) {
      return 1;
    }
  }
  if (s1.name < s2.name) {
    return -1;
  }
  if (s1.name > s2.name) {
    return 1;
  }
  return 0;
}

bool symbol::operator==(const symbol& o) const {
  return compare(*this, o) == 0;
}

bool symbol::operator<(const symbol& o) const {
  return compare(*this, o) < 0;
}

bool symbol::operator>(const symbol& o) const {
  return compare(*this, o) > 0;
}

std::ostream& operator<<(std::ostream& out, std::shared_ptr<symbol> o) {
  if (o == nullptr) {
    out << object::nil;
  } else {
    out << o->get_name();
  }
  return out;
}
