#ifndef _SYMBOL_H
#define _SYMBOL_H

// symbol class
class symbol : public object {
public:
  static std::shared_ptr<symbol> create(const std::string &name);
  static std::shared_ptr<symbol> create(const std::string &ns, 
                                        const std::string &name);
  static std::unique_ptr<symbol> create_unique(const std::string &name);
  static std::unique_ptr<symbol> create_unique(const std::string &ns, 
                                               const std::string &name);

  std::string get_name();
  bool operator==(const object& o) const override;
  bool operator==(const symbol& o) const;
  bool operator<(const symbol& o) const;
  bool operator>(const symbol& o) const;
  friend int compare(const symbol& s1, const symbol& s2);
  //friend std::ostream& operator<<(std::ostream& out, std::shared_ptr<symbol> o);
  friend class keyword;

  // TODO: i'm going to make the constructor here public
  // but it may be better to do something like: http://stackoverflow.com/a/8147326
  // however, since there's no interning of strings in c++, which is
  // what i see as the only real advantage to forcing creation of symbols
  // through "create" in java clojure, i don't think it's really necessary
  symbol(const std::string &ns, const std::string &name);
  symbol(const std::string &name);

  // TODO: there are no default initializer_list constructors
  // this symbol{} does not work
  std::string to_string() const override;
  size_t hash_code() const override;

protected:
  std::string ns;
  std::string name;
  size_t hash;

  // TODO: i couldn't seem to get this to work
  //friend std::unique_ptr<symbol> make_unique<symbol>( std::string ns, std::string name );
  //friend std::shared_ptr<symbol> std::make_shared<symbol>( std::string ns, std::string name );
private:
  void compute_hash();
};

#endif
