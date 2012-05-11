#ifndef _NAMESPACE_H
#define _NAMESPACE_H

#include <atomic>

// namespace class
class Namespace : public Object {
public:
  static std::shared_ptr<Namespace> findOrCreate(const std::shared_ptr<Symbol> &name);
  static std::shared_ptr<Namespace> findOrCreate(const std::string &name);

  Namespace(const std::shared_ptr<Symbol> &name);

  std::shared_ptr<Symbol> getName() const;
  std::string toString() const override;

  bool instanceof(const std::type_info &info) const override;

  std::shared_ptr<Object> getMapping(std::shared_ptr<Symbol> name);
private:
  // TODO: atomic (although it may be the case that shared_ptrs are already atomic?)
  std::shared_ptr<Map> mappings;
  std::shared_ptr<Map> aliases;

  std::shared_ptr<Symbol> name;
};

#endif
