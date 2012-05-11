#include "clojure.h"

List::List() 
  : _first(Object::nil), _count(0) {}

List::List(std::shared_ptr<Object> first) 
  : _first(first), _count(1) {}

List::List(std::shared_ptr<Object> first, std::shared_ptr<List> rest) 
  : _first(first), _rest(rest), _count(1) {
  if (_rest != Object::nil) {
    _count = 1 + _rest->count();
  }
}

List::List(const std::list<std::shared_ptr<Object> > &init) 
  : _first(Object::nil), _count(0) {
  if (!init.empty()) {
    auto it = init.rbegin();
    _first = *it++;
    while (it != init.rend()) {
      _rest = std::make_shared<List>(_first, _rest);
      _first = *it++;
    }
    _count = 1 + (_rest == Object::nil ? 0 : _rest->count());
  }
}

List::List(std::shared_ptr<Map> meta, std::shared_ptr<Object> first, std::shared_ptr<List> rest)
  : _meta(meta), _first(first), _rest(rest) {}


List::List(std::initializer_list<std::shared_ptr<Object> > init) 
  : List(std::list<std::shared_ptr<Object> >(init)) {
  // TODO: this doesn't seem like it would be very efficient
}

size_t List::count() const {
  return _count;
}

std::shared_ptr<Object> List::first() const {
  return _first;
}

std::shared_ptr<Seq> List::rest() const {
  return _rest;
}

std::shared_ptr<Seq> List::cons(std::shared_ptr<Object> o) const {
  return std::make_shared<List>( o,
                                 std::make_shared<List>(_first, _rest) );
}

std::string List::toString() const {
  return utils::print_string( shared_from_this() );
}

bool List::instanceof(const std::type_info &info) const {
  return (
          typeid(Seq) == info ||
          typeid(List) == info
          );
}

std::shared_ptr<Map> List::meta() const {
  return _meta;
}

std::shared_ptr<Object> List::withMeta(std::shared_ptr<Map> meta) const {
  return std::make_shared<List>(meta, _first, _rest);
}

