#include <functional>
#include "clojure.h"

// TODO: thoughts on const& vs copys
// this function doesn't store its own copy of name so it
// makes sense to not copy the string here. but the symbol
// constructor makes a copy of the string for storage
std::shared_ptr<symbol> symbol::create(const std::string& name) {
  return symbol::create_unique(name);
}

std::shared_ptr<symbol> symbol::create(const std::string &ns, 
                                       const std::string &name) {
  return std::make_shared<symbol>(ns, name);
}

std::unique_ptr<symbol> symbol::create_unique(const std::string &name) {
  return make_unique<symbol>(name);
}

std::unique_ptr<symbol> symbol::create_unique(const std::string &ns, 
                                              const std::string &name) {
  return make_unique<symbol>(ns, name);
}

std::string symbol::get_name() {
  return name;
}

std::string symbol::to_string() const {
  if (ns != "") {
    return ns + "/" + name;
  } else {
    return name;
  }
}

size_t symbol::hash_code() const {
  return hash;
}

symbol::symbol(const std::string &name) {
  size_t slashpos = name.rfind("/");
  if (slashpos != std::string::npos) {
    this->ns = name.substr(0, slashpos-1);
    this->name = name.substr(slashpos+1);
  } else {
    this->ns = "";
    this->name = std::string{ name };
  }
  compute_hash();
}

symbol::symbol(const std::string &ns, const std::string &name) {
  // TODO: is passing in const& and then explicitly copying the string
  // really necessary? if i'm copying it, is it better to let the object
  // get copied. how do i tell if this->ns = (a non-const ns) uses move
  // semantics rather than creating a whole new object?
  this->ns = std::string{ ns }; // TODO: do i really need to be specific here?
  this->name = std::string{ name };
  compute_hash();
}

void symbol::compute_hash() {
  size_t name_hash = std::hash<std::string>()(name);
  size_t ns_hash = 0;
  if (ns != "") {
    ns_hash = std::hash<std::string>()(ns);
  }
  this->hash = ns_hash ^ name_hash + 0x9e3779b9 + (ns_hash << 6) + (ns_hash >> 2);
}

bool symbol::operator==(const object& o) const {
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

/*
std::ostream& operator<<(std::ostream& out, std::shared_ptr<symbol> o) {
  if (o == nullptr) {
    out << object::nil;
  } else {
    out << o->get_name();
  }
  return out;
}
*/
