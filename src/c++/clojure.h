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
class object {
public:
  static std::shared_ptr<object> nil;
  static std::shared_ptr<object> T;
  static std::shared_ptr<object> F;

  virtual std::string to_string() const = 0;
  // TODO: make this = 0 and implement it for every object type
  virtual size_t hash_code() const { return 0; }

  friend std::ostream& operator<<(std::ostream& out, const object& o);
  virtual bool operator==(const object& o) const;
protected:
  object() {};
};

#include "boolean.h"
#include "string.h"

#include "symbol.h"
#include "keyword.h"
#include "namespace.h"

#include "numbers.h"

#include "seqs.h"

#endif
