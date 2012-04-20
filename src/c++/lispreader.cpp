#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include "clojure.h"
#include "utf8.h"
#include "lispreader.h"

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

obj read_string(std::istream &in) {
  std::stringstream buf;
  std::function<int ()> getc = // lambda to get the next char, or throw EOF exception
    [&in] () { 
    int c = in.get();
    if (in.eof()) {
      throw "EOF while reading string";
    }
    return c;
  };
  while (1) {
    int c = getc();
    if (c == '"') {
      return std::make_shared<string>(buf.str());
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
  return object::nil;
}

using macro_fn = obj(*)(std::istream &);

macro_fn getmacro(int c) {
  if (c == '"') {
    return read_string;
  } else {
    return 0;
  }
}

bool isterminator(int c) {
  // TODO: this should be modified once the macros array is set up
  return c == ')' || c == '}' || c == ']';
}

enum class number_type : int {
  none,
  integer,
  irrational,
  scientific,
  ratio
};

obj read_number(std::istream &in) {
  std::stringstream buf;
  std::string start = "";
  bool error = false;
  number_type type = number_type::none;
  int base = 10;

  std::function<bool (int)> isend = // lambda to tell if we've reached the end of the number
    [&in] (int i) { return iswhitespace(i) || isterminator(i) || in.eof(); };

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
          return std::make_shared<integer>(buf.str(), base);
        } else if (type == number_type::irrational || type == number_type::scientific) {
          return std::make_shared<irrational>(buf.str());
        } else if (type == number_type::ratio) {
          std::string r = buf.str();
          size_t s = r.find('/');
          return std::make_shared<ratio>(r.substr(0, s), r.substr(s+1));
        }
      }
      return object::nil; // should never get here
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
          base = stoi(radix);
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
  return object::nil;
}

obj read_token(std::istream &in) {
  std::stringstream buf;
  std::string ns = "";
  for (; ;) {
    int c = in.get();
    if (in.eof() || iswhitespace(c) || (c != '#' && getmacro(c) != 0)) {
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
    return object::nil;
  }
  if (s == "true") {
    return object::T;
  }
  if (s == "false") {
    return object::F;
  }
  // TODO: / = slash, clojure.core// = slash
  if (ns != "" && ns.substr(ns.size()-3) == ":/"
      || s.back() == ':'
      || s.find("::", 1) != std::string::npos) {
    return nullptr;
  }
  if (s[0] == ':' && s[1] == ':') {
    auto ks = symbol::create(s.substr(2));
    // TODO: handle namespace qualified keywords
    return nullptr;
  }
  bool iskey = s[0] == ':';
  // TODO: do we need intern?
  auto sym = symbol::create(iskey ? s.substr(1) : s);
  if (iskey) {
    return keyword::create(sym);
  }
  return sym;
}

obj lispreader::read(std::istream &in, bool eof_is_error, 
                     obj eof_value, bool is_recursive) {

  for (; ;) {
    int c;
    while (iswhitespace(c = in.get())) {}
    if (in.eof()) {
      return eof_value;
    } else {
      if (c == '+' || c == '-') {
        c = in.get();
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
