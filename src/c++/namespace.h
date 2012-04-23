#ifndef _NAMESPACE_H
#define _NAMESPACE_H

// namespace class
class Namespace : public Object {
public:
  static std::shared_ptr<Namespace> findOrCreate(const Symbol &name);
  static std::shared_ptr<Namespace> findOrCreate(const std::string &name);

  Namespace(const Symbol &name);

  Symbol getName() const;
  std::string toString() const override;

private:
  // mappings
  // aliases
  Symbol name;
};

#endif
