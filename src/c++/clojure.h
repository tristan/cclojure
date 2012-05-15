#ifndef _CLOJURE_H
#define _CLOJURE_H

#include <string>
#include <memory>
#include <boost/any.hpp>

namespace clojure {
  using object = std::shared_ptr<boost::any>;
  using symbol = std::pair<std::string,std::string>;
  struct keyword {
    keyword(symbol &&sym) : sym(sym) {};
    symbol sym;
  };

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

#endif
