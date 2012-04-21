#ifndef _KEYWORD_H
#define _KEYWORD_H

// keyword class
class keyword : public object {
public:
  static std::shared_ptr<keyword> create(const symbol& name);
  static std::shared_ptr<keyword> create(const std::string& name);
  static std::shared_ptr<keyword> create(const std::string &ns, 
                                         const std::string &name);
  // TODO: note that this is private in java clojure
  keyword(const symbol& name);

  std::string to_string() const override;
  size_t hash_code() const override;
private:
  // TODO: does the storage of a symbol here simply to allow
  // namespace qualified keywords?
  symbol sym;
  size_t hash;
};

#endif
