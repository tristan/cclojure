#ifndef _NAMESPACE_H
#define _NAMESPACE_H

// namespace class
// TODO: look into naming conventions
// using capital here because namespace is a keyword in c++
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
