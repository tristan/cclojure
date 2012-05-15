#include <functional>
#include <memory>
#include "clojure.h"
#include "runtime.h"
#include "compiler.h"

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

std::map<std::shared_ptr<Symbol>, std::shared_ptr<Object> > specials {
  std::make_pair(Compiler::DEF, nullptr),
    std::make_pair(Compiler::LOOP, nullptr),
    std::make_pair(Compiler::RECUR, nullptr)
    };

enum class C {
  STATEMENT,
    EXPRESSION,
    RETURN,
    EVAL
    };

/*
class Expr {
  std::shared_ptr<Object> eval() = 0;
};
*/

using Expr = std::function<std::shared_ptr<Object>()>;

Expr analyzeSymbol(std::shared_ptr<Symbol> sym) {
  // TODO: implement
  return [] () -> std::shared_ptr<Object> {
    return nullptr;
  };
}

Expr registerKeyword(std::shared_ptr<Keyword> keyword) {
  // TODO: implement
  return [] () -> std::shared_ptr<Object> {
    return nullptr;
  };
}

Expr analyze(C context, std::shared_ptr<Object> form, const std::string &name) {
  // TODO: implemnt all cases
  if (form == nullptr) {
    return [] () -> std::shared_ptr<Object> {
      return nullptr;
    };
  } else if (form == Object::T) {
    return [] () -> std::shared_ptr<Object> {
      return Object::T;
    };
  } else if (form == Object::F) {
    return [] () -> std::shared_ptr<Object> {
      return Object::F;
    };
  }

  if (form->instanceof(typeid(Symbol))) {
    return analyzeSymbol(std::dynamic_pointer_cast<Symbol>(form));
  } else if (form->instanceof(typeid(Keyword))) {
    return registerKeyword(std::dynamic_pointer_cast<Keyword>(form));
  }

  // TODO: implemnt real default
  return [] () -> std::shared_ptr<Object> {
    return nullptr;
  };
}

Expr analyze(C context, std::shared_ptr<Object> form) {
  return analyze(context, form, "");
}

bool Compiler::isSpecial(std::shared_ptr<Object> sym) {
  if (!sym->instanceof(typeid(Symbol))) {
    return false;
  }
  auto it = specials.find(std::dynamic_pointer_cast<Symbol>(sym));
  return (it != specials.end());
}

std::shared_ptr<Object> Compiler::eval(std::shared_ptr<Object> form) {
  Var::pushThreadBindings(std::make_shared<Map>(std::list<std::shared_ptr<Object> >{LOADER, nullptr}));
  return nullptr;

  // push new class loader onto thread stack

  // get line number for "form" if one exists
  // and push it onto the thread stack

  // do macro expansion of "form"

  // if "form" is a collection, and starts with "do"
  //   iterate over the collection and eval all the
  //   items in the collection, only returning the
  //   results of the final evaluation

  // if "form" is a collection and the first item is
  // a symbol which starts with "def"
  //   build an object expression
  //   eval that expression

  // else ...
  //   build an expression from "form"
  //   eval the expression
  
}

std::shared_ptr<Symbol> Compiler::resolveSymbol(std::shared_ptr<Symbol> sym) {
  // TODO: implement
  return sym;
}

std::shared_ptr<Namespace> Compiler::currentNS() {
  return std::dynamic_pointer_cast<Namespace>(runtime::CURRENT_NS->deref());
}
