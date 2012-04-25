#include "clojure.h"

// TODO: concurrency
std::map<std::shared_ptr<Symbol>,std::shared_ptr<Namespace>,compare_hashcodes> namespaces;
// is there a case where a namespace has a namespace?
// is this why the name is a symbol and not a string?

Namespace::Namespace(const std::shared_ptr<Symbol> &name) : name(name) {
  // TODO: mappings
  // TODO: aliases
}

std::shared_ptr<Namespace> Namespace::findOrCreate(const std::string &name) {
  return Namespace::findOrCreate(Symbol::create(name));
}

std::shared_ptr<Namespace> Namespace::findOrCreate(const std::shared_ptr<Symbol> &name) {
  auto it = namespaces.find(name);
  if (it != namespaces.end()) {
    return it->second;
  }
  auto ns = std::make_shared<Namespace>(name);
  namespaces[name] = ns;
  return ns;
}

std::shared_ptr<Symbol> Namespace::getName() const {
  return name;
}

std::string Namespace::toString() const {
  return name->getName();
}

bool Namespace::instanceof(const std::type_info &info) const {
  return (
          typeid(Namespace) == info
          );
}
