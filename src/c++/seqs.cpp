#include "clojure.h"

/* Clojure is quite complex here..
   i'm going to just implement this as needed
   and see where it goes

const bool Aseq::counted = false;

std::shared_ptr<Aseq> Aseq::cons(obj o) {
  return std::shared_ptr<Aseq>( new Cons(o, nullptr) );
}

std::shared_ptr<Aseq> Aseq::more() {
  std::shared_ptr<Aseq> s = next();
  if (s == nullptr) {
    // return empty seq
  }
  return s;
}

int Aseq::count() {
  int i = 1;
  for (std::shared_ptr<Aseq> s = next(); s != nullptr; s = s->next(), i++) {
    // NOTE: it would seem easier to support constant time count by returning
    // i + s->count() every time. but this means the call stack would grow
    // and we'd probably overflow for large lists
    // TODO: is this really a problem to watch out for in c++ ?
    if (s->counted) {
      return i + s->count();
    }
  }
  return i;
}

std::shared_ptr<Aseq> Aseq::seq() {
  // TODO: hmmm, what does this actually mean?
  // does this simply add an extra reference to "this" ?
  return std::shared_ptr<Aseq>(this);
}


Cons::Cons(obj first, std::shared_ptr<Aseq> more) {
  this->_first = first;
  this->_more = more;
}

obj Cons::first() {
  return _first;
}

std::shared_ptr<Aseq> Cons::next() {
  return more();
}

std::shared_ptr<Aseq> Cons::more() {
  if (_more == nullptr) {
    // TODO: return empty seq
  }
  return _more;
}

int Cons::count() {
  // TODO: RT.count is a bit more complex
  // NOTE: see http://blog.higher-order.net/2010/10/14/vectormap-and-pvectormap/
  // for explination of Util.ret1 function in clojure
  return 1 + (_more == nullptr ? 0 : _more->count());
}

class EmptyList : public Alist {
  virtual obj first() override {
    return nullptr;
  }
  virtual std::shared_ptr<Aseq> next() override {
    return nullptr;
  }
  virtual std::shared_ptr<Aseq> more() override {
    return nullptr;
  }
  virtual int count() override {
    return 0;
  }
  std::shared_ptr<Alist> cons(obj o) {
    return std::shared_ptr<Alist>( new PersistentList(o) );
  }
  friend class PersistentList;
private:
  EmptyList();
protected:
  static const bool counted = true;
};

const bool PersistentList::counted = true;

std::shared_ptr<Alist> PersistentList::EMPTY =
  std::shared_ptr<Alist>( new EmptyList() );

PersistentList(obj first, std::shared_ptr<Alist> rest, int count) {
    this->_first = first;
  this->_rest = rest;
  this->_count = count;
}

PersistentList(obj first) {
  this->_first = first;
  this->_rest = nullptr;
  this->_count = 1;
}

obj PersistentList::first() {
  return _first;
}

std::shared_ptr<Aseq> PersistentList::next() {
  if (_count == 1) {
    return nullptr;
  }
  return _rest;
}

std::shared_ptr<Alist> PersistentList::cons(obj o) {
  return this;
}

std::shared_ptr<Alist> PersistentList::create(std::list<obj> &init) {
  std::shared_ptr<Alist> ret = PersistentList::EMPTY;
  for (auto i = init.rbegin() ; i != init.rend() ; ++i) {
    // TODO: uh...
    ret.reset(ret->cons(*i).get());
  }
  return ret;
}

*/
