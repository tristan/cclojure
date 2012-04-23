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
    _count = 1 + _rest->count();
  }
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

// TODO: most of the code here should be done by a generic function
// that the repl can call when it has a "seq" type
std::string List::toString() const {
  return utils::print_string( shared_from_this() );
}
