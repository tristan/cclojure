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

std::string keyword::to_string() {
  return ":" + sym.to_string();
}

keyword::keyword(const symbol& sym) : sym(sym) {
  // TODO: experiment: do i need to specifically copy here
  // or does non-const = const& automatically fire the copy constructor?
  // explicitly implement the copy and see if it gets called normally
  //this->sym = symbol(sym);
  hash = this->sym.hash + 0x9e3779b9;
}
