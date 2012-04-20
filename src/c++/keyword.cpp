#include <functional>
#include "clojure.h"

// TODO: fix this, it's not going to have the desired effect
// unless the symbols inside the pointers are compared
std::map<std::shared_ptr<symbol>,std::shared_ptr<keyword>> keyword::table;

std::shared_ptr<keyword> keyword::create(std::string name) {
  return keyword::create(symbol::create(name));
}

std::shared_ptr<keyword> keyword::create(std::string ns, std::string name) {
  return keyword::create(symbol::create(ns, name));
}

std::shared_ptr<keyword> keyword::create(std::shared_ptr<symbol> sym) {
  auto it = table.find(sym);
  if (it != table.end()) {
    return it->second;
  }
  auto kw = std::shared_ptr<keyword>( new keyword(sym) );
  table[sym] = kw;
  return kw;
}

std::string keyword::to_string() {
  return ":" + sym->to_string();
}

keyword::keyword(std::shared_ptr<symbol> sym) {
  this->sym = sym;
  hash = sym->hash + 0x9e3779b9;
}
