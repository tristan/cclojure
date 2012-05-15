#include "clojure.h"
#include "runtime.h"

namespace runtime {
  std::shared_ptr<Namespace> CLOJURE_NS = Namespace::findOrCreate("clojure.core");
  std::shared_ptr<Var> CURRENT_NS = std::make_shared<Var>(CLOJURE_NS,
                                                          Symbol::create("*ns*"),
                                                          true);
}

std::atomic_uint __id( 1 );

int runtime::nextId() {
  return __id++;
}

std::shared_ptr<Seq> seqFrom(std::shared_ptr<Object> coll) {
  if (coll == nullptr) {
    return nullptr;
  } else if (coll->instanceof(typeid(Seqable))) {
    return std::dynamic_pointer_cast<Seqable>(coll)->seq();
  }
  throw std::string{"Don't know how to create seq from "} + typeid(coll.get()).name();
}

std::shared_ptr<Seq> runtime::seq(std::shared_ptr<Object> coll) {
  return seqFrom(coll);
}

std::shared_ptr<Object> runtime::first(std::shared_ptr<Object> x) {
  if (x->instanceof(typeid(Seq))) {
    return std::dynamic_pointer_cast<Seq>(x)->first();
  }
  return nullptr;
  // TODO: real seq impl
}

std::shared_ptr<Object> runtime::second(std::shared_ptr<Object> x) {
  return first(std::dynamic_pointer_cast<Object>(next(x)));
}

std::shared_ptr<Seq> runtime::next(std::shared_ptr<Object> x) {
  if (x->instanceof(typeid(Seq))) {
    return std::dynamic_pointer_cast<Seq>(x)->rest();
  }
  return nullptr;
  // TODO: real seq impl
}



/** list support **/

std::shared_ptr<Seq> runtime::list(std::shared_ptr<Object> arg1, std::shared_ptr<Object> arg2) {
  // TODO: figure out if we need to follow the was java clojure does this
  auto l = std::make_shared<List>(arg2);
  return l->cons(arg1);
}
