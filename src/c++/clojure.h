#ifndef _CLOJURE_H
#define _CLOJURE_H

#include <string>
#include <memory>
#include <map>

class object;

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

// TODO: look into boost::any as an alternative here
// which seems to offer us the ability to avoid making extra
// wrapping classes for string and boolean etc, but may
// lead to some extra overhead doing type deduction.
// http://www.boost.org/doc/libs/1_40_0/boost/any.hpp

// TODO: Thoughts on supporting inheritence
// Possible ways to represent the common things like Comparable, Seqable, IMeta, IFn, etc.,
// 1. enum { INTERFACE_Comparable, INTERFACE_Seqable, .... }
//  - possibly stored bitwise per class
// 2. object has virtual functions for each interface that the classes can implement to return
//    true if they implement that interface
//   p: removes dependence of typeid and casting
// 3. have all classes implement "instanceof(type_info &)"

// base object class
// TODO: how much overhead is added by enable_shared_from_this ?
// should i only turn this on for objects that really need it ?
class Object : public std::enable_shared_from_this<Object> {
public:
  static std::shared_ptr<Object> nil;
  static std::shared_ptr<Object> T;
  static std::shared_ptr<Object> F;

  virtual std::string toString() const = 0;
  // TODO: make this = 0 and implement it for every Object type
  virtual size_t hashCode() const;

  friend std::ostream& operator<<(std::ostream& out, const Object& o);

  virtual bool operator==(const Object& o) const;

  // interface stuff
  virtual bool iscomparable() const { return false; };
  virtual bool isseqable() const { return false; };
  virtual bool hasmetadata() const { return false; };
  virtual bool iscallable() const { return false; };
  virtual bool isaseq() const { return false; };
  virtual bool isamap() const { return false; };
  virtual bool isavector() const { return false; };
  virtual bool isaset() const { return false; };
  virtual bool isalist() const { return false; };
  virtual bool isassociative() const { return isamap() || isavector(); };
  virtual bool isastack() const { return isalist() || isavector(); };
  virtual bool isacollection() const { return isassociative() || isaset() || isaseq() || isastack(); };

  // TODO: make this = 0 to force classes to implement it
  virtual bool instanceof(const std::type_info &info);
};

// interface class defines needed for other interfaces
class Seq;
class Map;

// special interfaces
class Comparable : public virtual Object {
public:
  bool iscomparable() const override final { return true; }

  virtual int compareTo(const Object& o) const = 0;

  virtual bool operator<(const Object& o) const;
  virtual bool operator>(const Object& o) const;
  virtual bool operator==(const Object& o) const;
};

class Seqable : public virtual Object {
public:
  bool isseqable() const override final { return true; };
  virtual std::shared_ptr<Seq> seq() const = 0;
};

// note that java allows changing of return types to derived types
// but c++ doesn't, so the seq class can't inherit from the collection class 
// in this state
// TODO: is the Collection interface really needed?
// TODO: perhaps all the inheritence stuff is really unnecessary, and using typeid() is ok!
class Collection : public virtual Seqable {
  virtual size_t count() const = 0;
  virtual std::shared_ptr<Collection> cons(std::shared_ptr<Object> o) const = 0;
  virtual std::shared_ptr<Collection> empty() const = 0;
  virtual bool equiv(std::shared_ptr<Object> o) const = 0;

  bool isacollection() const override final { return true; };
};


// base seq object
class Seq : public virtual Object {
public:
  virtual std::shared_ptr<Object> first() const = 0;
  virtual std::shared_ptr<Seq> rest() const = 0;
  virtual std::shared_ptr<Seq> cons(std::shared_ptr<Object> o) const = 0;
  virtual size_t count() const = 0;

  bool isaseq() const override final { return true; };
};

// specifies that a class can hold metadata
// TODO: perhaps the implementation can be done here too, but this is assuming
// most of the  implementations are the same, perhaps they're not!
class Meta : public virtual Object {
public:
  virtual std::shared_ptr<Map> meta() const = 0;
  virtual std::shared_ptr<Object> withMeta(std::shared_ptr<Map> meta) const = 0;

  virtual bool hasmetadata() const override final { return true; };
};

struct compare_object_in_shared_ptr {
  bool operator()(const std::shared_ptr<Comparable>& lhs, const std::shared_ptr<Comparable>& rhs) const;
};
struct compare_hashcodes {
  bool operator()(const std::shared_ptr<Object>& lhs, const std::shared_ptr<Object>& rhs) const;
};

#include "boolean.h"
#include "string.h"
#include "character.h"
#include "regex.h"

#include "symbol.h"
#include "keyword.h"
#include "namespace.h"

#include "numbers.h"

#include "list.h"
#include "vector.h"
#include "map.h"
#include "set.h"

//#include "seqs.h"
#include "utils.h"

#endif
