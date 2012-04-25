#include "clojure.h"

Vector::Vector() {}

Vector::Vector(std::shared_ptr<Seq> items) {
  for (; items != nullptr ; items = items->rest()) {
    vec.push_back(items->first());
  }
}

Vector::Vector(const std::list<std::shared_ptr<Object> > &init) {
  auto it = init.begin();
  while (it != init.end()) {
    vec.push_back(*it++);
  }
}

size_t Vector::count() const {
  return vec.size();
}

std::shared_ptr<Object> Vector::first() const {
  if (vec.size() > 0) {
    return vec[0];
  }
  return Object::nil;
}

std::shared_ptr<Seq> Vector::rest() const {
  if (vec.size() > 1) {
    std::shared_ptr<Vector> r = std::make_shared<Vector>();
    r->vec = std::vector<std::shared_ptr<Object> >{ vec.begin()+1, vec.end() };
    return r;
  }
  // TODO: can't use Object::nil here because it's not a valid Seq
  // however, maybe we should return an empty seq instead?
  // or perhaps i should just start using nullptr instead of
  // Object::nil everywhere
  return nullptr;
}

std::shared_ptr<Seq> Vector::cons(std::shared_ptr<Object> o) const {
  std::shared_ptr<Vector> r = std::make_shared<Vector>( *this );
  r->vec.push_back(o);
  return r;
}

std::string Vector::toString() const {
  return utils::print_string( shared_from_this() );
}

bool Vector::instanceof(const std::type_info &info) const {
  return (
          typeid(Vector) == info ||
          typeid(Seq) == info
          );
}
