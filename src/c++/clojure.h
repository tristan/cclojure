#ifndef _CLOJURE_H
#define _CLOJURE_H

//#include <cstddef>
#include <string>
#include <memory>

class object;

using obj = std::shared_ptr<object>;

class object {
public:
  static obj nil;

  friend std::ostream& operator<<(std::ostream& out, obj o);
  virtual bool operator==(const object& o);
protected:
  object() {}; // we shouldn't even instantiate an object
  virtual std::string to_string();
};

class string : public object {
public:
  string(std::string s);
  bool operator==(const object &o);
protected:
  std::string str;
  std::string to_string() override;
};

#include "numbers.h"

bool operator==(obj o1, obj o2);

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

#endif
