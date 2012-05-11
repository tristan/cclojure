#ifndef _CLJ_UTILS_H
#define _CLJ_UTILS_H

namespace utils {

  extern std::shared_ptr<Namespace> CLOJURE_NS;
  extern std::shared_ptr<Var> CURRENT_NS;

  void print(const std::shared_ptr<const Object> &o, std::ostream &out);
  std::string print_string(const std::shared_ptr<const Object> &o);

  std::shared_ptr<Seq> seq(const std::shared_ptr<const Object> &o);

  int nextId();
}

#endif
