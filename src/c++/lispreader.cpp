#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <list>
#include <regex>
#include <map>
#include "clojure.h"
#include "utf8.h"

using namespace clojure;

static inline bool iswhitespace(int c) {
  return std::isspace(c) || c == ',';
}

static inline int octal_digit(char c)
{
    return (c >= '0' && c <= '7');
}

static inline int hex_digit(char c)
{
    return ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'F') ||
            (c >= 'a' && c <= 'f'));
}


object QUOTE = make_symbol("quote");
object THE_VAR = make_symbol("var");
object UNQUOTE = make_symbol("clojure.core", "unquote");
object UNQUOTE_SPLICING = make_symbol("clojure.core", "unquote-splicing");

object SLASH = make_symbol("/");
object CLOJURE_SLASH = make_symbol("clojure.core", "/");

// a special ptr for macros that have no return value
object NOOP = make_symbol("noop");

// gensymbol
//std::shared_ptr<Var> GENSYM_ENV = Var::createDynamic(nullptr);
//std::shared_ptr<Var> ARG_ENV = Var::createDynamic(nullptr);

// externs for macro_fn
object read_string(std::istream &in);
object read_list(std::istream &in);
object read_vector(std::istream &in);
object read_map(std::istream &in);
object read_set(std::istream &in);
object read_number(std::istream &in);
object read_comment(std::istream &in);
object read_character(std::istream &in);
object read_regex(std::istream &in) { throw "TODO: regex"; };
object read_meta(std::istream &in) { throw "TODO: meta"; };

object wrap_read(std::istream &in, object sym) {
  auto o = read(in, true, NIL, true);
  return make_object(std::list<object> { sym, o });
}

/*

bool is_unquote(std::shared_ptr<Object> form) {
  if (form->instanceof(typeid(Seq))) {
    return utils::equals(runtime::first(form), UNQUOTE);
  }
  return false;
}

bool is_unquote_splicing(std::shared_ptr<Object> form) {
  if (form->instanceof(typeid(Seq))) {
    return utils::equals(runtime::first(form), UNQUOTE_SPLICING);
  }
  return false;
}
*/

// TODO: can we replace macro_fn with std::function ?
using macro_fn = object(*)(std::istream &);

macro_fn getmacro(int c) {
  if (c == '"') {
    return read_string;
  } else if (c == '(') {
    return read_list;
  } else if (c == ')') {
    // lambda's are awesome!
    // but! note that this is only ok as long as the capture list is empty
    // TODO: and i should probably make sure this doesn't incur any
    // run time overhead as apposed to using pure functions
    return [] (std::istream &) -> object {
      throw "Unmatched delimiter: )";
    };
  } else if (c == '[') {
    return read_vector;
  } else if (c == ']') {
    return [] (std::istream &) -> object {
      throw "Unmatched delimiter: ]";
    };
  } else if (c == '{') {
    return read_map;
  } else if (c == '}') {
    return [] (std::istream &) -> object {
      throw "Unmatched delimiter: }";
    };
  } else if (c == '\'') {
    return [] (std::istream &in) -> object {
      return wrap_read(in, make_symbol("quote"));
    };
  } else if (c == '@') {
    return [] (std::istream &in) -> object {
      return wrap_read(in, make_symbol("clojure.core", "deref"));
    };
  } else if (c == ';') {
    return read_comment;
  } else if (c == '\\') {
    return read_character;
  } else if (c == '^') {
    return read_meta;
  } else if (c == '`') {
    return [] (std::istream &in) -> object {
      throw "TODO: implement syntax quote reader";
    };
  } else if (c == '~') {
    // unquote reader
    return [] (std::istream &in) -> object {
      int ch = in.get();
      if (in.eof()) {
        throw "EOF while reading character";
      }
      if (ch == '@') {
        return wrap_read(in, UNQUOTE_SPLICING);
      } else {
        in.unget();
        return wrap_read(in, UNQUOTE);
      }
    };
  }  else if (c == '%') {
    return [] (std::istream &in) -> object {
      throw "TODO: implement arg reader";
    };
  } else if (c == '#') {
    // dispatch macro reader (lambda)
    // TODO: is this over-using lambdas? what is the overhead of a lambda
    // over a pure function, esp if the function is being called via a ptr
    return [] (std::istream &in) -> object {
      int c = in.get();
      if (in.eof()) {
        throw "EOF while reading character";
      }
      if (c == '{') {
        return read_set(in);
      } else if (c == '^') {
        return read_meta(in);
      } else if (c == '\'') {
        return wrap_read(in, THE_VAR);
      } else if (c == '"') {
        return read_regex(in);
      } else if (c == '(') {
        throw "TODO: implement function reader";
      } else if (c == '=') {
        throw "TODO: implement eval reader";
      } else if (c == '!') {
        return read_comment(in);
      } else if (c == '<') {
        throw "Unreadable form";
      } else if (c == '_') {
        read(in, true, NIL, true);
        return NOOP;
      } else {
        // try ctor reader
        // TODO: implement ctor reader
      }
      throw "No dispatch macro for: " + std::to_string((char)c);
    };
  } else {
    return 0;
  }
}

inline bool isterminator(int c) {
  return c != '#' && getmacro(c) != 0;
}

long read_unicode_char(const std::string &token, int offset, int length, int base) {
  if (token.size() != offset + length) {
    throw "Invalid unicode character: \\" + token;
  }
  long uc = 0;
  for (int i = offset; i < offset + length; ++i) {
    long d = std::stol(token.substr(i, 1), 0, base);
    if (d == -1) {
      throw "Invalid digit: " + token.substr(i, 1);
    }
    uc = uc * base + d;
  }
  return uc;
}

/*

std::shared_ptr<Object> syntax_quote(std::shared_ptr<Object> form) {
  std::shared_ptr<Object> ret = nullptr;
  if (Compiler::isSpecial(form)) {
    ret = std::shared_ptr<List>( new List { Compiler::QUOTE, form } );
  } else if (form->instanceof(typeid(Symbol))) {
    std::shared_ptr<Symbol> sym = std::dynamic_pointer_cast<Symbol>(form);
    if (sym->ns == "" && sym->name.back() == '#') {
      auto gmap = std::dynamic_pointer_cast<Map>(GENSYM_ENV->deref());
      if (gmap == nullptr) {
        throw "Gensym literal not in syntax-quote";
      }
      auto gs = std::dynamic_pointer_cast<Symbol>(gmap->valAt(sym));
      if (gs == nullptr) {
        gs = Symbol::create("", sym->name.substr(0, sym->name.size() - 1) +
                            "__" + std::to_string(runtime::nextId()) + "__auto__");
        GENSYM_ENV->set(gmap->assoc(sym, gs));
      }
      sym = gs;
    } else if (sym->ns == "" && sym->name.back() == '.') {
      auto csym = Symbol::create("", sym->name.substr(0, sym->name.size() - 1));
      csym = Compiler::resolveSymbol(csym);
      sym = Symbol::create("", csym->name + ".");
    } else if (sym->ns == "" && sym->name.front() == '.') {
      // simply quote method names
    } else {
      std::shared_ptr<Object> maybeClass = nullptr;
      if (sym->ns != "") {
        maybeClass = Compiler::currentNS()->getMapping(Symbol::create("", sym->ns));
      }
      if (maybeClass != nullptr) {
        if (false) {
          // TODO: we have nothing to represent Class objects yet
        } else {
          sym = Compiler::resolveSymbol(sym);
        }
      }
    }
    ret = std::shared_ptr<List>( new List { Compiler::QUOTE, sym } );
  } else if (is_unquote(form)) {
    return runtime::second(form);
  } else if (is_unquote_splicing(form)) {
    throw "splice not in list";
  } else if (false) { // TODO: instanceof IPersistentCollection
  } else if (form->instanceof(typeid(Keyword)) ||
             form->instanceof(typeid(Number)) ||
             form->instanceof(typeid(Character)) ||
             form->instanceof(typeid(String))) {
    ret = form;
  } else {
    ret = std::dynamic_pointer_cast<Object>(runtime::list(Compiler::QUOTE, form));
  }
  return ret;
}

*/

object read_character(std::istream &in) {
  std::stringstream buf;
  int c = in.get();
  if (in.eof()) {
    throw "EOF while reading character";
  }
  buf.put(c);
  for (; ;) {
    c = in.get();
    if (in.eof() || iswhitespace(c) || isterminator(c)) {
      in.unget();
      break;
    }
    buf.put(c);    
  }
  std::string token = buf.str();
  if (token.size() == 1) {
    return make_object( token[0] );
  } else if (token == "newline") {
    return make_object( '\n' );
  } else if (token == "space") {
    return make_object( ' ' );
  } else if (token == "tab") {
    return make_object( '\t' );
  } else if (token == "backspace") {
    return make_object( '\b' );
  } else if (token == "formfeed") {
    return make_object( '\f' );
  } else if (token == "return") {
    return make_object( '\r' );
  } else if (token[0] == 'u') {
    long uc = read_unicode_char(token, 1, 4, 16);
    if (c >= 0xD800 && c <= 0xDFFF) {
      // TODO: java clojure actually prints u + the hex value of uc
      // is this any different than token?
      throw "Invalid character constant: \\" + token;
    }
    return make_object( uc );
  } else if (token[0] == 'o') {
    int len = token.size() - 1;
    if (len > 3) {
      throw "Invalid octal escape sequence length: " + std::to_string(len);
    }
    long uc = read_unicode_char(token, 1, len, 8);
    if (uc > 0377) {
      throw "Octal escape sequence mst be in range [0, 377].";
    }
    return make_object( uc );
  }
  throw "Unsupported character: \\" + token;
}

object read_string(std::istream &in) {
  std::stringstream buf;
  // lambda to get the next char, or throw EOF exception
  std::function<int ()> getc = [&in] () -> int { 
    int c = in.get();
    if (in.eof()) {
      throw "EOF while reading string";
    }
    return c;
  };
  while (1) {
    int c = getc();
    if (c == '"') {
      return make_object(buf.str());
    }
    if (c == '\\') {
      c = getc();
      if (octal_digit(c)) {
        int j = 0;
        char eseq[4] = { 0, 0, 0, 0 } ;
        do {
          eseq[j++] = c;
          c = getc();
        } while (octal_digit(c) && j < 3);
        in.unget();
        if (!octal_digit(c) && j < 3) {
          if (c == '\\') {
          } else {
            buf.str("Invalid digit: ");
            buf.put(c);
            throw buf.str();
          }
        }
        u8_wc_toutf8(buf, std::strtol(eseq, 0, 8));
      } else if (c == 'u') {
        int j = 0;
        char eseq[5] = { 0, 0, 0, 0, 0 };
        c = getc();
        while (hex_digit(c) && j < 4) {
          eseq[j++] = c;
          c = getc();
        }
        if (j != 4) {
          buf.str("Invalid character length: ");
          buf << j << ", should be: 4";
          throw buf.str();
        }
        in.unget();
        long uni = std::strtol(eseq, 0, 16);
        u8_wc_toutf8(buf, uni);
      } else {
        if (c == 'b') {
          buf.put('\b');
        } else if (c == 'n') {
          buf.put('\n');
        } else if (c == 't') {
          buf.put('\t');
        } else if (c == 'f') {
          buf.put('\f');
        } else if (c == 'r') {
          buf.put('\r');
        } else if (c == '"') {
          buf.put('\'');
        } else if (c == '\\') {
          buf.put('\\');
        } else {
          buf.str("Unsupported escape character: \\");
          buf.put(c);
          throw buf.str();
        }
      }
    } else {
      buf.put(c);
    }
  }
  return NIL;
}

void read_delimited_list(int delim, std::istream &in, std::function<void(object)> putfn) {
  for (; ;) {
    int c;
    while (iswhitespace(c = in.get())) {}

    if (in.eof()) {
      throw "EOF while reading";
    }
    
    if (c == delim) {
      break;
    }

    macro_fn fn = getmacro(c);
    if (fn != 0) {
      auto m = fn(in);
      if (m != NOOP) {
        putfn(m);
      }
    } else {
      in.unget();
      auto o = read(in, true, NIL, true);
      if (o != NOOP) {
        putfn(o);
      }
    }
  }
}

object read_list(std::istream &in) {
  std::list<object> list;
  read_delimited_list(')', in,
                      [&list] (object o) -> void {
                        list.push_back(o);
                      });
  return make_object(list);
}

object read_vector(std::istream &in) {
  std::vector<object> vec;
  read_delimited_list(']', in,
                      [&vec] (object o) -> void {
                        vec.push_back(o);
                      });
  return make_object(vec);
}

object read_set(std::istream &in) {
  std::set<object> set;
  read_delimited_list('}', in,
                      [&set] (object o) -> void {
                        auto r = set.insert(o);
                            if (r.second == false) {
                              throw "Duplicate key: " + to_string(o);
                            }
                      });
  return make_object(set);
}


object read_map(std::istream &in) {
  std::map<object,object> map;
  object key = nullptr;
  read_delimited_list('}', in,
                      [&map, &key] (object o) -> void {
                        if (key == nullptr) {
                          key = o;
                        } else {
                          auto r = map.insert(std::make_pair(key,o));
                          if (r.second == false) {
                            throw "Duplicate key: " + to_string(o);
                          }
                          key = nullptr;
                        }
                      });

  if (key != nullptr) {
    throw "Map literal must contain an even number of forms";
  }
  return make_object( map );
}

object read_comment(std::istream &in) {
  int c;
  do {
    c = in.get();
  } while (!in.eof() && c != '\n' && c != '\r');
  return NOOP;
}

/*

std::shared_ptr<Object> read_regex(std::istream &in) {
  std::stringstream buf;
  std::function<int ()> getc = [&in] () -> int { 
    int c = in.get();
    if (in.eof()) {
      throw "EOF while reading regex";
    }
    return c;
  };
  while (1) {
    int c = getc();
    if (c == '"') {
      return std::make_shared<Pattern>(buf.str());
    }
    buf.put(c);
    if (c == '\\') {
      c = getc();
      buf.put(c);
    }
  }
  return Object::nil;
}

std::shared_ptr<Object> read_meta(std::istream &in) {
  auto meta = read(in, true, Object::nil, true);
  if (typeid(*meta) == typeid(Symbol) || typeid(*meta) == typeid(String)) {
    meta = std::make_shared<Map>(std::list<std::shared_ptr<Object>>{Keyword::create("tag"), meta});
  } else if (typeid(*meta) == typeid(Keyword)) {
    meta = std::make_shared<Map>(std::list<std::shared_ptr<Object>>{meta, Object::T});
  } else if (typeid(*meta) != typeid(Map)) { // TODO: map interface
    throw "Metadata must be Symbol,Keyword,String or Map";
  }
  auto o = read(in, true, Object::nil, true);
  if (o->instanceof(typeid(Meta))) {
    // TODO: line number support
    // TODO: refs
    // merge metadata
    auto ometa = std::dynamic_pointer_cast<Meta>(o)->meta();
    for (auto it = std::dynamic_pointer_cast<Map>(meta)->begin(); 
        it != std::dynamic_pointer_cast<Map>(meta)->end();
        ++it) {
      auto r = *it;
      if (ometa == nullptr) {
        ometa = std::make_shared<Map>(std::list<std::shared_ptr<Object> >{ std::get<0>(r), std::get<1>(r) });
      } else {
        ometa = std::dynamic_pointer_cast<Map>(ometa)->assoc(std::get<0>(r), std::get<1>(r));
      }
    }
    return std::dynamic_pointer_cast<Meta>(o)->withMeta(ometa);
  }
  throw "Metadata can only be applied to IMetas";
}

*/

enum class number_type : int {
  none,
  integer,
  irrational,
  scientific,
  ratio
};

object read_number(std::istream &in) {
  std::stringstream buf;
  std::string start = "";
  bool error = false;
  number_type type = number_type::none;
  int base = 10;

  // lambda to tell if we've reached the end of the number
  std::function<bool (int)> isend = [&in] (int i) -> bool { 
    return iswhitespace(i) || isterminator(i) || in.eof(); 
  };

  int c;
  // process the first few chars
  while (!isend(c = in.get())) {
    if (c == '0') { // if the first character is 0, there's only a few posible options
      c = in.get();
      if (isend(c)) { // check if it's 0
        type = number_type::integer;
        buf.put('0');
        break;
      } else if (c == 'x' || c == 'X') { // check if it's a hex value
        type = number_type::integer;
        base = 16;
        start += "0";
        start += (char)c;
        break;
      } else if (c >= '0' && c <= '7') { // check if it's an oct value
        type = number_type::integer;
        buf.put(c);
        base = 8;
        start = "0";
        break;
      } else if (c == 'e' || c == 'E') {
        type = number_type::scientific;
        buf.put('0');
        buf.put(c);
        break;
      } else {
        error = true;
        buf << "O" << (char)c;
        break;
      }
    } else if (c == '-') { // add the sign to the buf if a -
      buf.put(c);
    } else if (c == '+') {
      // ignore the sign if it's a +
    } else {
      buf.put(c);
      break;
    }
  }
  while (1) {
    c = in.get();
    if (isend(c)) {
      // TODO: process buf and get the value
      in.unget(); // TODO: make sure we want to unget here
      if (error) {
        buf.str("Invalid Number: " + start + buf.str());
        throw buf.str();
      } else {
        if (type == number_type::integer || type == number_type::none) {
          return make_object(std::stoi(buf.str(), 0, base));
        } else if (type == number_type::irrational || type == number_type::scientific) {
          return make_object(std::stod(buf.str(), 0));
        } else if (type == number_type::ratio) {
          std::string r = buf.str();
          size_t s = r.find('/');
          return make_object(std::make_pair(std::stoi(r.substr(0, s), 0, 10),
                                            std::stoi(r.substr(s+1), 0, 10)));
        }
      }
      return NIL; // should never get here
    }
    if (error) {
      buf.put(c);
    } else if (c == 'r') {
      if (type != number_type::none) { // we already have a base, so this is an invalid number
        error = true;
        buf.put(c);
      } else {
        std::string radix = buf.str();
        buf.str("");
        if (radix.size() > 2) {
          error = true;
          buf.put(c);
        } else {
          base = std::stoi(radix, 0, 10);
          if (base > 36) {
            // TODO: NumberFormatException
            throw "Radix out of range";
          }
          type = number_type::integer;
        }
      }
    } else if (c == '/') {
      buf.put(c);
      if (type != number_type::none) { // we already have a base, so this is an invalid number
        error = true;
      } else {
        type = number_type::ratio;
        c = in.get();
        if (isend(c)) {
          error = true;
          in.unget();
        } else {
          buf.put(c);
        }
      }
    } else if (c == '.' && type == number_type::none) {
      type = number_type::irrational;
      buf.put(c);
    } else if ((c == 'e' || c == 'E') && 
               (type == number_type::none || type == number_type::irrational)) {
      type = number_type::scientific;
      buf.put(c);
      c = in.get();
      if (isend(c)) {
        error = true;
      } else {
        buf.put(c);
        if (!(c == '-' || c == '+' || (c >= '0' && c <= '9'))) {
          error = true;
        }
      }
    } else if ((c == 'N' || c == 'M') && base == 10) { // M and N endings only possible with base 10
      c = in.get();
      if (!isend(c)) {
        error = true;
        buf.put(c);
      } else {
        in.unget();
      }
    } else if (c >= '0' && ((base < 11 && (c < '0' + base)) || 
                            (base > 10 && (c <= '9' || 
                                           (c >= 'a' && c < 'a' + (base-10)) ||
                                           (c >= 'A' && c < 'A' + (base-10))
                                           )))) {
      buf.put(c);
    } else {
      buf.put(c);
      error = true;
    }
  }
  return NIL;
}

object read_token(std::istream &in) {
  std::stringstream buf;
  std::string ns = "";
  for (; ;) {
    int c = in.get();
    if (in.eof() || iswhitespace(c) || isterminator(c)) {
      in.unget();
      break;
    }
    buf.put(c);
    if (c == '/') {
      ns = buf.str();
    }
  }
  std::string s = buf.str();
  if (s.back() == '/') {
    // TODO: this is the only case i can spot where we have an invalid token
    throw "Invalid token: " + s;
  }
  if (s == "nil") {
    return NIL;
  }
  if (s == "true") {
    return TRUE;
  }
  if (s == "false") {
    return FALSE;
  }
  if (s == "/") {
    return SLASH;
  }
  if (s == "clojure.core//") {
    return CLOJURE_SLASH;
  }
  if ((ns != "" && ns.substr(ns.size()-3) == ":/")
      || s.back() == ':'
      || s.find("::", 1) != std::string::npos) {
    return NIL;
  }
  if (s[0] == ':' && s[1] == ':') {
    auto ks = make_symbol(s.substr(2));
    // TODO: handle namespace qualified Keywords
    return NIL;
  }
  bool iskey = s[0] == ':';
  if (iskey) {
    return make_keyword(s.substr(1));
  }
  return make_symbol(s);
}

object clojure::read(std::istream &in, bool eof_is_error, 
                     object eof_value, bool is_recursive) {
  for (; ;) {
    int c;
    while (iswhitespace(c = in.get())) {}
    if (in.eof()) {
      return eof_value;
    } else {
      if (c == '+' || c == '-') {
        int c2 = in.peek();
        if (std::isdigit(c2)) {
          in.unget();
          return read_number(in);
        }
      }
      if (std::isdigit(c)) {
        in.unget();
        return read_number(in);
      }
      macro_fn fn = getmacro(c);
      if (fn != 0) {
        return fn(in);
      }
      in.unget();
      return read_token(in);
      throw "not yet supported";
    }
  }

  return eof_value;
}
