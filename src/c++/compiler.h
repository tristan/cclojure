#ifndef _CLJ_COMPILER_H
#define _CLJ_COMPILER_H

class Compiler {
public:
  static std::shared_ptr<Object> eval(std::shared_ptr<Object> form);

  static bool isSpecial(std::shared_ptr<Object> sym);

  static std::shared_ptr<Symbol> resolveSymbol(std::shared_ptr<Symbol> sym);
  static std::shared_ptr<Namespace> currentNS();

public: // symbols
  static std::shared_ptr<Symbol> DEF;
  static std::shared_ptr<Symbol> LOOP;
  static std::shared_ptr<Symbol> RECUR;
  static std::shared_ptr<Symbol> IF;
  static std::shared_ptr<Symbol> LET;
  static std::shared_ptr<Symbol> LETFN;
  static std::shared_ptr<Symbol> DO;
  static std::shared_ptr<Symbol> FN;
  static std::shared_ptr<Symbol> QUOTE;
  static std::shared_ptr<Symbol> THE_VAR;
  static std::shared_ptr<Symbol> DOT;
  static std::shared_ptr<Symbol> ASSIGN;
  static std::shared_ptr<Symbol> TRY;
  static std::shared_ptr<Symbol> CATCH;
  static std::shared_ptr<Symbol> FINALLY;
  static std::shared_ptr<Symbol> THROW;
  static std::shared_ptr<Symbol> MONITOR_ENTER;
  static std::shared_ptr<Symbol> MONITOR_EXIT;
  static std::shared_ptr<Symbol> IMPORT;
  static std::shared_ptr<Symbol> DEFTYPE;
  static std::shared_ptr<Symbol> CASE;
  static std::shared_ptr<Symbol> CLASS;
  static std::shared_ptr<Symbol> NEW;
  static std::shared_ptr<Symbol> THIS;
  static std::shared_ptr<Symbol> REIFY;
  static std::shared_ptr<Symbol> LIST;
  static std::shared_ptr<Symbol> HASHMAP;
  static std::shared_ptr<Symbol> VECTOR;
  static std::shared_ptr<Symbol> IDENTIRY;
  static std::shared_ptr<Symbol> _AMP_;
  static std::shared_ptr<Symbol> ISEQ;
};

#endif 
