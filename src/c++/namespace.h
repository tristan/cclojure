#ifndef _NAMESPACE_H
#define _NAMESPACE_H

// namespace class
// TODO: look into naming conventions
// using capital here because namespace is a keyword in c++
class Namespace : public object {
public:
  static std::shared_ptr<Namespace> findOrCreate(std::shared_ptr<symbol> name);
  std::shared_ptr<symbol> get_name();
  std::string to_string() const override;
private:
  Namespace(std::shared_ptr<symbol> name);
  // TODO: make sure access to this is thread safe (which it's not by default)
  static std::map<std::shared_ptr<symbol>,std::shared_ptr<Namespace>> namespaces;
  // mappings
  // aliases
  std::shared_ptr<symbol> name;
};

#endif
