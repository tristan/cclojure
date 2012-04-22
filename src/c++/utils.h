#ifndef _CLJ_UTILS_H
#define _CLJ_UTILS_H

namespace utils {
  void print(const std::shared_ptr<Object> &o, std::ostream &out);
  void print(const Object *o, std::ostream &out);
  std::string print_string(const std::shared_ptr<Object> &o);
  std::string print_string(const Object *o);
}

#endif
