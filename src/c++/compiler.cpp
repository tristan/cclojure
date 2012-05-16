#include <functional>
#include <memory>
#include <list>
#include <map>
#include <string>
#include <set>
#include <vector>
#include <iostream>
#include "clojure.h"
#include "runtime.h"
#include "util.h"
#include "compiler.h"

namespace compiler {
  clojure::object DO = clojure::make_symbol("do");
}

/*
std::shared_ptr<Var> LOADER = Var::createDynamic();
std::shared_ptr<Var> KEYWORDS = Var::createDynamic();

std::shared_ptr<Symbol> Compiler::DEF = Symbol::create("def");
std::shared_ptr<Symbol> Compiler::LOOP = Symbol::create("loop*");
std::shared_ptr<Symbol> Compiler::RECUR = Symbol::create("recur");
std::shared_ptr<Symbol> Compiler::IF = Symbol::create("if");
std::shared_ptr<Symbol> Compiler::LET = Symbol::create("let*");
std::shared_ptr<Symbol> Compiler::LETFN = Symbol::create("letfn*");
std::shared_ptr<Symbol> Compiler::DO = Symbol::create("do");
std::shared_ptr<Symbol> Compiler::FN = Symbol::create("fn*");
std::shared_ptr<Symbol> Compiler::QUOTE = Symbol::create("quote");
std::shared_ptr<Symbol> Compiler::THE_VAR = Symbol::create("var");
std::shared_ptr<Symbol> Compiler::DOT = Symbol::create(".");
std::shared_ptr<Symbol> Compiler::ASSIGN = Symbol::create("set!");
std::shared_ptr<Symbol> Compiler::TRY = Symbol::create("try");
std::shared_ptr<Symbol> Compiler::CATCH = Symbol::create("catch");
std::shared_ptr<Symbol> Compiler::FINALLY = Symbol::create("finally");
std::shared_ptr<Symbol> Compiler::THROW = Symbol::create("throw");
std::shared_ptr<Symbol> Compiler::MONITOR_ENTER = Symbol::create("monitor-enter");
std::shared_ptr<Symbol> Compiler::MONITOR_EXIT = Symbol::create("monitor-exit");
std::shared_ptr<Symbol> Compiler::IMPORT = Symbol::create("clojure.core", "import*");
std::shared_ptr<Symbol> Compiler::DEFTYPE = Symbol::create("deftype*");
std::shared_ptr<Symbol> Compiler::CASE = Symbol::create("case*");
std::shared_ptr<Symbol> Compiler::CLASS = Symbol::create("Class");
std::shared_ptr<Symbol> Compiler::NEW = Symbol::create("new");
std::shared_ptr<Symbol> Compiler::THIS = Symbol::create("this");
std::shared_ptr<Symbol> Compiler::REIFY = Symbol::create("reify*");
std::shared_ptr<Symbol> Compiler::LIST = Symbol::create("clojure.core", "list");
std::shared_ptr<Symbol> Compiler::HASHMAP = Symbol::create("clojure.core", "hash-map");
std::shared_ptr<Symbol> Compiler::VECTOR = Symbol::create("clojure.core", "vector");
std::shared_ptr<Symbol> Compiler::IDENTIRY = Symbol::create("clojure.core", "identity");
std::shared_ptr<Symbol> Compiler::_AMP_ = Symbol::create("&");
std::shared_ptr<Symbol> Compiler::ISEQ = Symbol::create("clojure.lang.ISeq");

*/

using namespace clojure;

using expr = std::function<object()>;
expr analyze(object const &form, std::string name);

expr analyze_symbol(object const &sym) {
  // TODO: impl
  return [sym] () -> object { return sym; };
}

expr register_keyword(object const &kwd) {
  // TODO: impl thread binding stuff
  return [kwd] () -> object { return kwd; };
}

expr analyze_seq(object const &form, std::string name) {
  // TODO: impl thread binding stuff
  object me = compiler::macroexpand1(form);
  if (me != form) {
    return analyze(me, name);
  }
  list fl = boost::any_cast<list>(*form);
  object op = fl.front();
  if (op == clojure::NIL) {
    throw "Can't call nil";
  }

  return [] () -> object { return clojure::NIL; };
}

expr analyze(object const &form, std::string name) {
  if (form->type() == typeid(symbol)) {
    return analyze_symbol(form);
  }
  if (form->type() == typeid(keyword)) {
    return register_keyword(form);
  }
  if (form->type() == typeid(list)) {
    return analyze_seq(form, name);
  }
  return [form] () -> object { // Constant/Literal Expr
    return form;
  };
}

object compiler::macroexpand1(object const &x) {
  // TODO: impl
  return x;
}

object compiler::macroexpand(object const &form) {
  object exf = compiler::macroexpand1(form);
  if (exf != form) {
    return compiler::macroexpand(exf);
  }
  return form;
}

object compiler::eval(object const &x) {
  object form = macroexpand(x);
  if (form->type() == typeid(list)) { // NOTE: ISeq = list
    list l = boost::any_cast<list>(*form);
    if (util::equals(l.front(), compiler::DO)) {
      auto it = l.begin();
      it++;
      for (; it != l.end(); it++) {
        eval(*it);
      }
      --it;
      return eval(*it);
    }
    if (!(l.front()->type() == typeid(symbol) &&
          [] (symbol &&sym) -> bool { // .startsWith("def")
            return (sym.name.size() >= 3 && sym.name.substr(0, 3) == "def");
          }(boost::any_cast<symbol>(*l.front())))) {
      // analyze the list to generate an expression
      expr ex = analyze(form, "eval" + std::to_string(runtime::next_id()));
      // eval the expression to generate a function
      // invoke the function and return the result
    }    
  }
  // anazlyse the object to generate an expression
  // return the evaluation of the expression
  return form;
}
