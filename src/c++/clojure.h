#ifndef _CLOJURE_H
#define _CLOJURE_H

#include <cstddef>

class object {
 public:
  bool operator==(const object &o2);
  friend std::ostream& operator<<(std::ostream& out, object& o);
  virtual std::string to_string();

  static object nil;
 protected:
  object() {}; // we shouldn't even instantiate an object
  std::string java_type;
};

#endif
