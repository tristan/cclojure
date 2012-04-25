#ifndef _KEYWORD_H
#define _KEYWORD_H

// Keyword class
class Keyword : public Object {
public:
  // These are needed for keyword, as we store the table
  // of keywords. this is to make sure :a always == :a
  // address wise
  static std::shared_ptr<Keyword> create(std::shared_ptr<Symbol> name);
  static std::shared_ptr<Keyword> create(const std::string& name);
  static std::shared_ptr<Keyword> create(const std::string &ns, 
                                         const std::string &name);

  // TODO: the constructors shoudn't be callable, but this
  // makes them unaccessible to make_shared
  Keyword(std::shared_ptr<Symbol> name);
  Keyword(const std::string &name);
  Keyword(const std::string &ns, const std::string &name);

  std::string toString() const override;
  size_t hashCode() const override;
  bool instanceof(const std::type_info &info) const override;
private:
  // TODO: does the storage of a symbol here simply to allow
  // namespace qualified Keywords?
  std::shared_ptr<Symbol> sym;
  size_t hash;
};

#endif
