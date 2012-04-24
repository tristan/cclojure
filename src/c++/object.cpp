#include <iostream>
#include "clojure.h"

std::shared_ptr<Object> Object::nil = nullptr;
std::shared_ptr<Object> Object::T = std::make_shared<Boolean>(true);
std::shared_ptr<Object> Object::F = std::make_shared<Boolean>(false);

std::ostream& operator<<(std::ostream& out, const Object& o) {
  out << o.toString();
  return out;
}

bool Object::operator==(const Object &o) const {
  // throw error here because it should be implemented for everything
  // BUT is this actually a requirement in java? TODO: look into how
  // .equals and .hashCode react in java when not specifically implemented
  // TODO: look into real string formatting
  throw std::string("operator== is not implemented for type: ") + typeid(o).name();

}

bool compare_object_in_shared_ptr::operator()(const std::shared_ptr<Object>& lhs, 
                                    const std::shared_ptr<Object>& rhs ) const {
  std::cout << "( TODO: FIX compare_object_in_shared_ptr impl )";
  // TODO: we really want to return lhs < rhs
  // but i've yet to figure out how to do that properly for all objects
  // NOTE however that this is only currently needed for Sets and Maps
  // but if the storage of those types changes to use something other than
  // the stl types, this may not be needed at all
  return !(*lhs == *rhs);
}
