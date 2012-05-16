#ifndef _CLOJURE_H
#define _CLOJURE_H

#include <string>
#include <memory>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <atomic>
#include <boost/any.hpp>

namespace clojure {
  using object = std::shared_ptr<boost::any>;
  struct objcomp {
    bool operator()(object const &lhs, object const &rhs) const;
  };

  using map = std::map<object,object,objcomp>;
  using set = std::set<object,objcomp>;
  using list = std::list<object>;
  using vector = std::vector<object>;
  using string = std::string;  

  struct symbol {
    symbol(string &&ns, string &&name) : ns(ns), name(name) {};
    string ns;
    string name;
  };
  struct var {
    var(symbol &&ns, symbol &&sym, object &&root) : ns(ns), sym(sym), root(root) {};
    symbol ns;
    symbol sym;
    object root;
  };
  struct keyword {
    keyword(symbol &&sym) : sym(sym) {};
    symbol sym;
  };
  struct ratio {
    ratio(int num, int den) : num(num), den(den) {};
    int num;
    int den;
  };

  extern std::map<object,object,objcomp> namespaces;

  object make_namespace(object const &sym);

  template <typename... Args>
  inline auto make_object(Args&&... args) -> decltype(std::make_shared<boost::any>(std::forward<Args>(args)...)) {
    return std::make_shared<boost::any>(std::forward<Args>(args)...);
  }

  inline auto make_symbol(std::string &&name) -> object {
    size_t slashpos = name.rfind("/");
    if (slashpos != std::string::npos) {
      return std::make_shared<boost::any>(symbol(name.substr(0, slashpos), 
                                                 name.substr(slashpos+1)));
    } else {
      return std::make_shared<boost::any>(symbol("", 
                                                 std::forward<std::string>(name)));
    }
  }

  inline auto make_symbol(std::string &&ns, std::string &&name) -> object {
    return std::make_shared<boost::any>(symbol(std::forward<std::string>(ns), 
                                               std::forward<std::string>(name)));
  }

  inline auto make_symbol(std::string const &name) -> object {
    return make_symbol(std::string(name));
  }

  inline auto make_symbol(std::string const &ns, std::string const &name) -> object {
    return make_symbol(std::string(ns), std::string(name));
  }

  inline auto make_keyword(std::string &&name) -> object {
    return make_object(keyword(symbol("", 
                                      std::forward<std::string>(name))));
  }

  inline auto make_keyword(std::string &&ns, std::string &&name) -> object {
    return make_object(keyword(symbol(std::forward<std::string>(ns), 
                                      std::forward<std::string>(name))));
  }

  inline auto make_keyword(std::string const &name) -> object {
    return make_object(keyword(symbol("", std::string(name))));
  }

  inline auto make_keyword(std::string const &ns, std::string const &name) -> object {
    return make_object(keyword(symbol(std::string(ns), std::string(name))));
  }

  inline auto make_var(object const &ns, object const &sym, object const &root) -> object {
    return make_object(var(boost::any_cast<symbol>(*ns), boost::any_cast<symbol>(*sym), object(root)));
  }

  object read(std::istream &in, 
              bool eof_is_error, 
              object eof_value, 
              bool is_recursive);

  std::string to_string(object const &o);

  extern object NIL;
  extern object TRUE;
  extern object FALSE;
}

//std::ostream &operator<<(std::ostream &out, clojure::object const &o);

bool operator==(clojure::object const &lhs, clojure::object const &rhs);
bool operator!=(clojure::object const &lhs, clojure::object const &rhs);

/*
bool operator()(clojure::object const &lhs, clojure::object const &rhs) {
  std::cout << "operator() ";
  return false;
  }*/

#endif
