#ifndef _CLJ_VAR_H
#define _CLJ_VAR_H

#include <atomic>

class Var : public Object {
public:
  Var(std::shared_ptr<Namespace> ns, std::shared_ptr<Symbol> sym);
  Var(std::shared_ptr<Namespace> ns, std::shared_ptr<Symbol> sym, std::shared_ptr<Object> root);
  Var(std::shared_ptr<Namespace> ns, std::shared_ptr<Symbol> sym, bool dynamic);
  Var(std::shared_ptr<Namespace> ns, std::shared_ptr<Symbol> sym, std::shared_ptr<Object> root, bool dynamic);
  static std::shared_ptr<Var> create();
  static std::shared_ptr<Var> create(std::shared_ptr<Object> root);
  static std::shared_ptr<Var> createDynamic();
  static std::shared_ptr<Var> createDynamic(std::shared_ptr<Object> root);
  static void pushThreadBindings(std::shared_ptr<Map> bindings);

  void setDynamic();
  bool isDynamic();

  std::shared_ptr<Object> set(std::shared_ptr<Object> val);
  std::shared_ptr<Object> deref();

  std::string toString() const;
  //size_t hashCode() const;
  bool instanceof(const std::type_info &info) const;

private:
  std::shared_ptr<Object> getThreadBinding();

private:
  std::shared_ptr<Symbol> sym;
  std::shared_ptr<Namespace> ns;
  std::atomic<bool> threadBound;
  std::shared_ptr<Object> root;
  bool dynamic = false;
};

#endif
