#ifndef _SYMBOL_H
#define _SYMBOL_H

// symbol class
class Symbol : public Object {
public:
  static std::shared_ptr<Symbol> create(const std::string &name);
  static std::shared_ptr<Symbol> create(const std::string &ns, 
                                        const std::string &name);
  static std::unique_ptr<Symbol> create_unique(const std::string &name);
  static std::unique_ptr<Symbol> create_unique(const std::string &ns, 
                                               const std::string &name);
  bool operator==(const Object& o) const override;
  bool operator==(const Symbol& o) const;
  bool operator<(const Symbol& o) const;
  bool operator>(const Symbol& o) const;
  friend int compare(const Symbol& s1, const Symbol& s2);
  //friend std::ostream& operator<<(std::ostream& out, std::shared_ptr<Symbol> o);
  friend class Keyword;

  // TODO: i'm going to make the constructor here public
  // but it may be better to do something like: http://stackoverflow.com/a/8147326
  // however, since there's no interning of strings in c++, which is
  // what i see as the only real advantage to forcing creation of Symbols
  // through "create" in java clojure, i don't think it's really necessary
  Symbol(const std::string &ns, const std::string &name);
  Symbol(const std::string &name);

  std::string getName() const;

  // TODO: there are no default initializer_list constructors
  // this Symbol{} does not work
  std::string toString() const override;
  size_t hashCode() const override;

protected:
  std::string ns;
  std::string name;
  size_t hash;

  // TODO: i couldn't seem to get this to work
  //friend std::unique_ptr<Symbol> make_unique<Symbol>( std::string ns, std::string name );
  //friend std::shared_ptr<Symbol> std::make_shared<Symbol>( std::string ns, std::string name );
private:
  void computeHash();
};

#endif
