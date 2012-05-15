#ifndef _CLJ_RUNTIME_H
#define _CLJ_RUNTIME_H

namespace runtime {

  extern std::shared_ptr<Namespace> CLOJURE_NS;
  extern std::shared_ptr<Var> CURRENT_NS;

  int nextId();

  std::shared_ptr<Seq> seq(std::shared_ptr<Object> coll);
  std::shared_ptr<Object> first(std::shared_ptr<Object> x);
  std::shared_ptr<Object> second(std::shared_ptr<Object> x);
  std::shared_ptr<Seq> next(std::shared_ptr<Object> x);

  /** list support **/

  std::shared_ptr<Seq> list(std::shared_ptr<Object> arg1, std::shared_ptr<Object> arg2);

}

#endif
