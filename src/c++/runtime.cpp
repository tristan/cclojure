#include "clojure.h"
#include "runtime.h"
#include <list>
#include <vector>
#include <map>
#include <atomic>
#include <set>
#include <string>

using namespace clojure;

namespace runtime {
  object CLOJURE_NS = make_namespace(make_symbol("clojure.core"));
  object CURRENT_NS = make_var(CLOJURE_NS, make_symbol("*ns*"), CLOJURE_NS);
}

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
  else if (x->type() == typeid(double)) {
    out << boost::any_cast<double>(*x);
  }
  else if (x->type() == typeid(ratio)) {
    ratio r = boost::any_cast<ratio>(*x);
    out << r.num << "/" << r.den;
  }
  else if (x->type() == typeid(string)) {
    out << '"' << boost::any_cast<string>(*x) << '"';
  }
  else if (x->type() == typeid(symbol)) {
    symbol sym = boost::any_cast<symbol>(*x);
    if (sym.ns != "") {
      out << sym.ns << "/";
    }
    out << sym.name;
  } else if (x->type() == typeid(keyword)) {
    keyword kwd = boost::any_cast<keyword>(*x);
    out << ":";
    if (kwd.sym.ns != "") {
      out << kwd.sym.ns << "/";
    }
    out << kwd.sym.name;
  } else if (x->type() == typeid(list)) {
    out << "(";
    list l = boost::any_cast<list>(*x);
    for (auto it = l.begin(); it != l.end(); ) {
      print(*it++, out);
      if (it != l.end()) {
        out << " ";
      }
    }
    out << ")";
  } else if (x->type() == typeid(vector)) {
    out << "[";
    vector l = boost::any_cast<vector>(*x);
    for (auto it = l.begin(); it != l.end(); ) {
      print(*it++, out);
      if (it != l.end()) {
        out << " ";
      }
    }
    out << "]";  
  } else if (x->type() == typeid(set)) {
    out << "#{(";
    set l = boost::any_cast<set>(*x);
    for (auto it = l.begin(); it != l.end(); ) {
      print(*it++, out);
      if (it != l.end()) {
        out << " ";
      }
    }
    out << "}";
  } else if (x->type() == typeid(map)) {
    out << "{";
    map l = boost::any_cast<map>(*x);
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

std::atomic_int __id ( 1 );

int runtime::next_id() {
  return __id++;
}
