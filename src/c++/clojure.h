#ifndef _CLOJURE_H
#define _CLOJURE_H

//#include <cstddef>
#include <string>
#include <memory>
#include <map>

class object;

bool operator==(std::shared_ptr<object> o1, std::shared_ptr<object> o2);

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

// TODO: look into boost::any as an alternative here
// which seems to offer us the ability to avoid making extra
// wrapping classes for string and boolean etc, but may
// lead to some extra overhead doing type deduction.
// http://www.boost.org/doc/libs/1_40_0/boost/any.hpp

// base object class
class object {
public:
  static std::shared_ptr<object> nil;
  static std::shared_ptr<object> T;
  static std::shared_ptr<object> F;

  friend std::ostream& operator<<(std::ostream& out, std::shared_ptr<object> o);
  virtual bool operator==(const object& o) const;
protected:
  object() {}; // we shouldn't even instantiate an object

  // TODO: maybe rename this to "print" to allow for a public toString style method
  virtual std::string to_string();
};

// string wrapper class
class string : public object {
public:
  string(const std::string& s);
  string(const string& s);
  bool operator==(const object &o) const override;
protected:
  // TODO: would making this const make it immutatable?
  // if i really want an immutable string should i just use c_str?
  std::string str;
  std::string to_string() override;
};

// boolean wrapper class
class boolean : public object {
public:
  boolean(bool b);
  bool operator==(const object& o) const override;
  std::string to_string() override;
protected:
  bool value;
};

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
  friend std::ostream& operator<<(std::ostream& out, std::shared_ptr<symbol> o);
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

protected:
  std::string ns;
  std::string name;
  size_t hash;

  // TODO: i couldn't seem to get this to work
  //friend std::unique_ptr<symbol> make_unique<symbol>( std::string ns, std::string name );
  //friend std::shared_ptr<symbol> std::make_shared<symbol>( std::string ns, std::string name );

  std::string to_string() override;
private:
  void compute_hash();
};

// keyword class
class keyword : public object {
public:
  static std::shared_ptr<keyword> create(const symbol& name);
  static std::shared_ptr<keyword> create(const std::string& name);
  static std::shared_ptr<keyword> create(const std::string &ns, 
                                         const std::string &name);
  // TODO: note that this is private in java clojure
  keyword(const symbol& name);
protected:
  std::string to_string() override;
private:
  // TODO: does the storage of a symbol here simply to allow
  // namespace qualified keywords?
  symbol sym;
  size_t hash;
};

// namespace class
// TODO: look into naming conventions
// using capital here because namespace is a keyword in c++
class Namespace : public object {
public:
  static std::shared_ptr<Namespace> findOrCreate(std::shared_ptr<symbol> name);
  std::shared_ptr<symbol> get_name();
protected:
  std::string to_string() override;
private:
  Namespace(std::shared_ptr<symbol> name);
  // TODO: make sure access to this is thread safe (which it's not by default)
  static std::map<std::shared_ptr<symbol>,std::shared_ptr<Namespace>> namespaces;
  // mappings
  // aliases
  std::shared_ptr<symbol> name;
};

#include "numbers.h"

#include "seqs.h"

#endif
