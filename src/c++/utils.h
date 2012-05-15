#ifndef _CLJ_UTILS_H
#define _CLJ_UTILS_H

namespace utils {

  void print(const std::shared_ptr<const Object> &o, std::ostream &out);
  std::string print_string(const std::shared_ptr<const Object> &o);

  bool equals(std::shared_ptr<Object> k1, std::shared_ptr<Object> k2);
}

#endif
