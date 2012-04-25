#include <functional>
#include "clojure.h"

std::shared_ptr<Symbol> Symbol::create(const std::string& name) {
  return Symbol::create_unique(name);
}

std::shared_ptr<Symbol> Symbol::create(const std::string &ns, 
                                       const std::string &name) {
  return std::make_shared<Symbol>(ns, name);
}

std::unique_ptr<Symbol> Symbol::create_unique(const std::string &name) {
  return make_unique<Symbol>(name);
}

std::unique_ptr<Symbol> Symbol::create_unique(const std::string &ns, 
                                              const std::string &name) {
  return make_unique<Symbol>(ns, name);
}

std::string Symbol::getName() const {
  return name;
}

std::string Symbol::toString() const {
  if (ns != "") {
    return ns + "/" + name;
  } else {
    return name;
  }
}

size_t Symbol::hashCode() const {
  return hash;
}

// TODO: thoughts on const& vs copys
// this function doesn't always store its own copy of name so 
// it makes sense to not copy the string here. but the Symbol
// (ns,name) constructor makes a copy of the strings for storage
Symbol::Symbol(const std::string &name) {
  size_t slashpos = name.rfind("/");
  if (slashpos != std::string::npos) {
    this->ns = name.substr(0, slashpos-1);
    this->name = name.substr(slashpos+1);
  } else {
    this->ns = "";
    this->name = std::string{ name };
  }
  computeHash();
}

// TODO: is passing in const& and then explicitly copying the string
// really necessary? if i'm copying it, is it better to let the object
// get copied. how do i tell if this->ns = (a non-const ns) uses move
// semantics rather than creating a whole new object?
Symbol::Symbol(const std::string &ns, const std::string &name) : ns(ns), name(name) {
  computeHash();
}

void Symbol::computeHash() {
  size_t name_hash = std::hash<std::string>()(name);
  size_t ns_hash = 0;
  if (ns != "") {
    ns_hash = std::hash<std::string>()(ns);
  }
  this->hash = ns_hash ^ name_hash + 0x9e3779b9 + (ns_hash << 6) + (ns_hash >> 2);
}

bool Symbol::operator==(const Object& o) const {
  if (typeid(o) != typeid(Symbol)) {
    return false;
  }
  const Symbol &s = dynamic_cast<const Symbol&>(o);
  return this->name == s.name && this->ns == s.ns;
}

int Symbol::compareTo(const Comparable& o) const {
  if (typeid(o) != typeid(Symbol)) {
    throw std::string(typeid(o).name()) + " cannot be cast to Symbol";
  }
  const Symbol &s = dynamic_cast<const Symbol&>(o);
  if (this->ns == "" && s.ns != "") {
    return -1;
  }
  if (this->ns != "") {
    if (s.ns == "") {
      return 1;
    }
    if (this->ns < s.ns) {
      return -1;
    }
    if (this->ns > s.ns) {
      return 1;
    }
  }
  if (this->name < s.name) {
    return -1;
  }
  if (this->name > s.name) {
    return 1;
  }
  return 0;
}

bool Symbol::instanceof(const std::type_info &info) const {
  return (
          typeid(Symbol) == info ||
          typeid(Comparable) == info
          );
}
