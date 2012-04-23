#ifndef _SYMBOL_H
#define _SYMBOL_H

// symbol class
class Symbol : public Object {
public:
  // TODO: i'm going to make the constructor here public
  // but it may be better to do something like: http://stackoverflow.com/a/8147326
  // however, since there's no interning of strings in c++, which is
  // what i see as the only real advantage to forcing creation of Symbols
  // through "create" in java clojure, i don't think it's really necessary
  Symbol(const std::string &ns, const std::string &name);
  Symbol(const std::string &name);
  // TODO: there are no default initializer_list constructors
  // thus Symbol{} does not work

  std::string getName() const;

  std::string toString() const override;
  size_t hashCode() const override;

  bool operator==(const Object& o) const override;

  // needed to allow using Symbols as keys in std::maps
  bool operator==(const Symbol& o) const;
  bool operator<(const Symbol& o) const;
  bool operator>(const Symbol& o) const;
  friend int compare(const Symbol& s1, const Symbol& s2);

  // I thought about removing these since they're simply helper functions
  // and don't provide any real value, but I came to the conclusion that
  // helper functions are actually ok!
  static std::shared_ptr<Symbol> create(const std::string &name);
  static std::shared_ptr<Symbol> create(const std::string &ns, 
                                        const std::string &name);
  static std::unique_ptr<Symbol> create_unique(const std::string &name);
  static std::unique_ptr<Symbol> create_unique(const std::string &ns, 
                                               const std::string &name);
protected:
  std::string ns;
  std::string name;
  size_t hash;
private:
  void computeHash();
};

#endif
