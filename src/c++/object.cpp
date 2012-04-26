#include <iostream>
#include "clojure.h"

std::shared_ptr<Object> Object::nil = nullptr;
std::shared_ptr<Object> Object::T = std::make_shared<Boolean>(true);
std::shared_ptr<Object> Object::F = std::make_shared<Boolean>(false);

std::ostream& operator<<(std::ostream& out, const Object& o) {
  out << o.toString();
  return out;
}

size_t Object::hashCode() const {
  return (size_t)this;
}

bool Object::operator==(const Object &o) const {
  // by default in java and instance is only .equals to itself
  // so i do the same here.
  return this == &o;
}

bool Object::instanceof(const std::type_info &info) {
  return (typeid(Object) == info);
}

/*int Object::compareTo(const Object &o) const {
  // this is a simple way to simulate the Comparaible interface in java
  throw std::string(typeid(this).name()) + " has no natural ordering (i.e. does not implement compareTo).";
  }*/

bool Comparable::operator<(const Object &o) const {
  return this->compareTo(o) < 0;
}

bool Comparable::operator>(const Object &o) const {
  return this->compareTo(o) > 0;
}

bool Comparable::operator==(const Object &o) const {
  return this->compareTo(o) == 0;
}

// Comparator for collection that use compareTo to sort/match the keys/values
bool compare_object_in_shared_ptr::operator()(const std::shared_ptr<Comparable>& lhs, 
                                              const std::shared_ptr<Comparable>& rhs) const {
  return lhs->compareTo(*rhs) < 0;
}

// Comparator for collection that use the hashCode to sort/match the keys/values
bool compare_hashcodes::operator()(const std::shared_ptr<Object>& lhs, 
                                   const std::shared_ptr<Object>& rhs) const {
  return (lhs->hashCode() < rhs->hashCode());
}
