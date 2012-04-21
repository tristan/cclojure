#include <functional>
#include <algorithm>
#include "clojure.h"

// TODO: concurrency
static std::map<symbol,std::shared_ptr<keyword>> table;

std::shared_ptr<keyword> keyword::create(const std::string& name) {
  return keyword::create(symbol(name));
}

std::shared_ptr<keyword> keyword::create(const std::string &ns, 
                                         const std::string &name) {
  return keyword::create(symbol(ns, name));
}

std::shared_ptr<keyword> keyword::create(const symbol& sym) {
  auto it = table.find(sym);
  if (it != table.end()) {
    return it->second;
  }
  auto kw = std::make_shared<keyword>(sym);
  // TODO: should i use sym or kw->sym here?
  // does sym get copied when used as a key here?
  // perhaps the symbol hash is a better key to use
  table[sym] = kw;
  return kw;
}

std::string keyword::to_string() const {
  return ":" + sym.to_string();
}

size_t keyword::hash_code() const {
  return hash;
}

// initialise needed because there's no default constructor
// for symbol thus this->sym can't be initialized
keyword::keyword(const symbol& sym) : sym(sym) {
  // TODO: experiment: do i need to specifically copy here
  // or does non-const = const& automatically fire the copy constructor?
  // explicitly implement the copy and see if it gets called normally
  hash = this->sym.hash + 0x9e3779b9;
}
