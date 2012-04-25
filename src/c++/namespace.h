#ifndef _NAMESPACE_H
#define _NAMESPACE_H

// namespace class
class Namespace : public Object {
public:
  static std::shared_ptr<Namespace> findOrCreate(const std::shared_ptr<Symbol> &name);
  static std::shared_ptr<Namespace> findOrCreate(const std::string &name);

  Namespace(const std::shared_ptr<Symbol> &name);

  std::shared_ptr<Symbol> getName() const;
  std::string toString() const override;

  bool instanceof(const std::type_info &info) const override;
private:
  // mappings
  // aliases
  std::shared_ptr<Symbol> name;
};

#endif
