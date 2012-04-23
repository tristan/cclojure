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
  virtual size_t hashCode() const { return 0; }

  friend std::ostream& operator<<(std::ostream& out, const Object& o);
  virtual bool operator==(const Object& o) const;
};

// base seq object
class Seq : public Object {
public:
  virtual std::shared_ptr<Object> first() const = 0;
  virtual std::shared_ptr<Seq> rest() const = 0;
  virtual std::shared_ptr<Seq> cons(std::shared_ptr<Object> o) const = 0;
  virtual size_t count() const = 0;
};

#include "boolean.h"
#include "string.h"
#include "character.h"

#include "symbol.h"
#include "keyword.h"
#include "namespace.h"

#include "numbers.h"

#include "list.h"

//#include "seqs.h"
#include "utils.h"

#endif
