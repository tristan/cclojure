#ifndef _KEYWORD_H
#define _KEYWORD_H

// Keyword class
class Keyword : public Object {
public:
  static std::shared_ptr<Keyword> create(const Symbol& name);
  static std::shared_ptr<Keyword> create(const std::string& name);
  static std::shared_ptr<Keyword> create(const std::string &ns, 
                                         const std::string &name);
  // TODO: note that this is private in java clojure
  Keyword(const Symbol& name);

  std::string toString() const override;
  size_t hashCode() const override;
private:
  // TODO: does the storage of a symbol here simply to allow
  // namespace qualified Keywords?
  Symbol sym;
  size_t hash;
};

#endif
