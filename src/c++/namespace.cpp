#include "clojure.h"

// TODO: since this is private and static, it probably doesn't need to be in the class definition
// and can simply be visible in this file only
std::map<std::shared_ptr<symbol>,std::shared_ptr<Namespace>> Namespace::namespaces;

Namespace::Namespace(std::shared_ptr<symbol> name) {
  this->name = name;
  // TODO: mappings
  // TODO: aliases
}

std::shared_ptr<Namespace> Namespace::findOrCreate(std::shared_ptr<symbol> name) {
  auto it = namespaces.find(name);
  if (it != namespaces.end()) {
    return it->second;
  }
  auto ns = std::shared_ptr<Namespace>( new Namespace(name) );
  namespaces[name] = ns;
  return ns;
}

std::shared_ptr<symbol> Namespace::get_name() {
  return this->name;
}

std::string Namespace::to_string() const {
  return name->get_name();
}
