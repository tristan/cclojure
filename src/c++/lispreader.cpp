#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include "clojure.h"
#include "lispreader.h"

static inline bool iswhitespace(int c) {
  return std::isspace(c) || c == ',';
}

bool isterminator(int c) {
  // TODO: this should be modified once the macros array is set up
  return c == ')' || c == '}' || c == ']';
}

enum class number_type : int {
  none,
  integer,
  decimal, // float is a keyword!
  scientific,
  ratio
};

object &read_number(std::istream &in) {
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
        std::cout << "Invalid Number: " << start << buf.str();
      } else {
        std::cout << "got number: " << buf.str() << " base: " << base;
      }
      return object::nil;
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
      type = number_type::decimal;
      buf.put(c);
    } else if ((c == 'c' || c == 'E') && (type == number_type::none || type == number_type::decimal)) {
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

object &lispreader::read(std::istream &in, bool eof_is_error, 
                         object &eof_value, bool is_recursive) {

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
      throw "not yet supported";
    }
  }

  return eof_value;
}
