#ifndef _CLOJURE_H
#define _CLOJURE_H

//#include <cstddef>
#include <string>
#include <memory>

class object {
 public:
  bool operator==(const object &o2);
  friend std::ostream& operator<<(std::ostream& out, std::shared_ptr<object> o);
  virtual std::string to_string();

  static std::shared_ptr<object> nil;
 protected:
  object() {}; // we shouldn't even instantiate an object
  std::string java_type;
};

#endif
