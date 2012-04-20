#ifndef _PERSISTENT_LIST_H
#define _PERSISTENT_LIST_H

/*
#include <list>

class Aseq : public object {
public:
  virtual obj first() = 0;
  virtual std::shared_ptr<Aseq> next() = 0;
  virtual std::shared_ptr<Aseq> more();
  virtual std::shared_ptr<Aseq> cons(obj o) final;
  virtual std::shared_ptr<Aseq> seq() final; // TODO: this simply exists for (seq X) right?
  virtual int count();
protected:
  // some interface related things
  static const bool counted; // = false (TODO: is it a bug that i can't set this here)
};

class Cons : public Aseq {
private:
  obj _first;
  std::shared_ptr<Aseq> _more;
public:
  Cons(obj first, std::shared_ptr<Aseq> more);
  virtual obj first() override;
  virtual std::shared_ptr<Aseq> next() override;
  virtual std::shared_ptr<Aseq> more() override;
  virtual int count() override;
};

class Alist : public Aseq {
public:
  virtual std::shared_ptr<Alist> cons(obj o) = 0;
};

class PersistentList : public Alist {
private:
  obj _first;
  std::shared_ptr<Alist> _rest;
  int _count;
  PersistentList(obj first, std::shared_ptr<Alist> rest, int count);
public:
  static std::shared_ptr<Alist> EMPTY;
  PersistentList(obj first);
  static std::shared_ptr<Alist> create(std::list<obj> &init);
  obj first();
  std::shared_ptr<Aseq> next();
  std::shared_ptr<Alist> cons(obj o);
protected:
  static const bool counted; // = true;
};

*/

#endif
