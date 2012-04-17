#ifndef _CLOJURE_H
#define _CLOJURE_H

#include <cstddef>

class object {
 public:
  object(std::string symbol);
  static object nil;
  bool operator==(const object &o2);
  friend std::ostream& operator<<(std::ostream& out, object& o);
 private:
  std::string symbol;
};

#endif
