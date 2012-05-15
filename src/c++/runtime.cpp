#include "clojure.h"
#include "runtime.h"
#include <list>
#include <vector>
#include <map>
#include <set>
#include <string>

using namespace clojure;

void runtime::print(object x, std::ostream &out) {
  //out << "[[" << x->type().name() << "]] ";
  if (x->type() == typeid(std::nullptr_t)) {
    out << "nil";
  }
  else if (x->type() == typeid(bool)) {
    out << (boost::any_cast<bool>(*x) ? "true" : "false");
  }
  else if (x->type() == typeid(int)) {
    out << boost::any_cast<int>(*x);
  }
  else if (x->type() == typeid(std::string)) {
    out << '"' << boost::any_cast<std::string>(*x) << '"';
  }
  else if (x->type() == typeid(symbol)) {
    symbol sym = boost::any_cast<symbol>(*x);
    if (sym.first != "") {
      out << sym.first << "/";
    }
    out << sym.second;
  } else if (x->type() == typeid(keyword)) {
    keyword kwd = boost::any_cast<keyword>(*x);
    out << ":";
    if (kwd.sym.first != "") {
      out << kwd.sym.first << "/";
    }
    out << kwd.sym.second;
  } else if (x->type() == typeid(std::list<object>)) {
    out << "(";
    std::list<object> l = boost::any_cast<std::list<object>>(*x);
    for (auto it = l.begin(); it != l.end(); ) {
      print(*it++, out);
      if (it != l.end()) {
        out << " ";
      }
    }
    out << ")";
  } else if (x->type() == typeid(std::vector<object>)) {
    out << "[";
    std::vector<object> l = boost::any_cast<std::vector<object>>(*x);
    for (auto it = l.begin(); it != l.end(); ) {
      print(*it++, out);
      if (it != l.end()) {
        out << " ";
      }
    }
    out << "]";  
  } else if (x->type() == typeid(std::set<object>)) {
    out << "#{(";
    std::set<object> l = boost::any_cast<std::set<object>>(*x);
    for (auto it = l.begin(); it != l.end(); ) {
      print(*it++, out);
      if (it != l.end()) {
        out << " ";
      }
    }
    out << "}";
  } else if (x->type() == typeid(std::map<object,object>)) {
    out << "{";
    std::map<object,object> l = boost::any_cast<std::map<object,object>>(*x);
    for (auto it = l.begin(); it != l.end(); ) {
      auto val = *it++;
      print(val.first, out);
      out << " ";
      print(val.second, out);
      if (it != l.end()) {
        out << ", ";
      }
    }
    out << "}";
  } else {
    out << "#<" << x->type().name() << " >";
  }
}
