#ifndef _CLOJURE_H
#define _CLOJURE_H

//#include <cstddef>
#include <string>
#include <memory>
#include <map>

class object;

using obj = std::shared_ptr<object>;

// base object class
class object {
public:
  static obj nil;
  static obj T;
  static obj F;

  friend std::ostream& operator<<(std::ostream& out, obj o);
  virtual bool operator==(const object& o);
protected:
  object() {}; // we shouldn't even instantiate an object

  // TODO: maybe rename this to "print" to allow for a public toString style method
  virtual std::string to_string();
};

// string class
class string : public object {
public:
  string(std::string s);
  bool operator==(const object &o) override;
protected:
  std::string str;
  std::string to_string() override;
};

// boolean class
class boolean : public object {
public:
  boolean(bool b);
  bool operator==(const object& o) override;
  std::string to_string() override;
protected:
  bool value;
};

// symbol class
class symbol : public object {
public:
  static std::shared_ptr<symbol> create(std::string name);
  static std::shared_ptr<symbol> create(std::string ns, std::string name);
  std::string get_name();
  bool operator==(const object& o) override;
  bool operator==(const symbol& o) const;
  bool operator<(const symbol& o) const;
  bool operator>(const symbol& o) const;
  friend int compare(const symbol& s1, const symbol& s2);
  friend std::ostream& operator<<(std::ostream& out, std::shared_ptr<symbol> o);
  friend class keyword;
protected:
  std::string ns;
  std::string name;
  size_t hash;

  symbol(std::string ns, std::string name);
  std::string to_string() override;
};

// keyword class
class keyword : public object {
public:
  static std::shared_ptr<keyword> create(std::shared_ptr<symbol> name);
  static std::shared_ptr<keyword> create(std::string name);
  static std::shared_ptr<keyword> create(std::string ns, std::string name);
protected:
  std::string to_string() override;
private:
  // static std::map<std::shared_ptr<symbol>,std::shared_ptr<keyword>> table;
  std::shared_ptr<symbol> sym;
  size_t hash;
  keyword(std::shared_ptr<symbol> name);
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

bool operator==(obj o1, obj o2);

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

#endif
