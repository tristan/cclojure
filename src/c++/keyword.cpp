#include <functional>
#include <algorithm>
#include "clojure.h"

// TODO: concurrency
static std::map<std::shared_ptr<Symbol>,std::shared_ptr<Keyword>,compare_object_in_shared_ptr> table;

std::shared_ptr<Keyword> Keyword::create(const std::string& name) {
  return Keyword::create(Symbol::create(name));
}

std::shared_ptr<Keyword> Keyword::create(const std::string &ns, 
                                         const std::string &name) {
  return Keyword::create(Symbol::create(ns, name));
}

std::shared_ptr<Keyword> Keyword::create(std::shared_ptr<Symbol> sym) {
  auto it = table.find(sym);
  if (it != table.end()) {
    return it->second;
  }
  auto kw = std::make_shared<Keyword>(sym);
  // TODO: should i use sym or kw->sym here?
  // does sym get copied when used as a key here?
  // perhaps the Symbol hash is a better key to use
  table[sym] = kw;
  return kw;
}

std::string Keyword::toString() const {
  return ":" + sym->toString();
}

size_t Keyword::hashCode() const {
  return hash;
}

bool Keyword::instanceof(const std::type_info &info) const {
  return (
          typeid(Keyword) == info
          );
}

// initialise needed because there's no default constructor
// for Symbol thus this->sym can't be initialized
Keyword::Keyword(std::shared_ptr<Symbol> sym) : sym(sym) {
  // TODO: experiment: do i need to specifically copy here
  // or does non-const = const& automatically fire the copy constructor?
  // explicitly implement the copy and see if it gets called normally
  hash = this->sym->hashCode() + 0x9e3779b9;
}

Keyword::Keyword(const std::string &name) 
  : Keyword(Symbol::create(name)) {}

Keyword::Keyword(const std::string &ns, const std::string &name)
  : Keyword(Symbol::create(ns, name)) {}
