#ifndef _CLJ_UTILS_H
#define _CLJ_UTILS_H

namespace utils {
  void print(const std::shared_ptr<const Object> &o, std::ostream &out);
  std::string print_string(const std::shared_ptr<const Object> &o);

  std::shared_ptr<Seq> seq(const std::shared_ptr<const Object> &o);
  bool supports_meta(const std::shared_ptr<const Object> &o);
}

#endif
