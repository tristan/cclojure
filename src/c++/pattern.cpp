#include "clojure.h"
#include <iostream>

void handle_regex_error(std::regex_error &e) {
  auto code = e.code();
  if (code == std::regex_constants::error_collate) {
    throw "The expression contains an invalid collating element name.";
  }
  if (code == std::regex_constants::error_ctype) {
    throw "The expression contains an invalid character class name.";
  }
  if (code == std::regex_constants::error_escape) {
    throw "The expression contains an invalid escaped character or a trailing escape.";
  }
  if (code == std::regex_constants::error_backref) {
    throw "The expression contains an invalid back reference.";
  }
  if (code == std::regex_constants::error_brack) {
    throw "The expression contains mismatched square brackets.";
  }
  if (code == std::regex_constants::error_paren) {
    throw "The expression contains mismatched parentheses.";
  }
  if (code == std::regex_constants::error_brace) {
    throw "The expression contains mismatched curly braces.";
  }
  if (code == std::regex_constants::error_badbrace) {
    throw "The expression contains an invalid range in a {} expression.";
  }
  if (code == std::regex_constants::error_range) {
    throw "The expression contains an invalid character range.";
  }
  if (code == std::regex_constants::error_space) {
    throw "There was not enough memory to convert the expression into a finite state machine.";
  }
  if (code == std::regex_constants::error_badrepeat) {
    throw "One of *?+{ was not preceded by a valid regular expression.";
  }
  if (code == std::regex_constants::error_complexity) {
    throw "The complexity of an attempted match exceeded a predefined level.";
  }
  if (code == std::regex_constants::error_stack) {
    throw "There was not enough memory to perform a match.";
  }
  throw e.what();
}

Pattern::Pattern(const std::string &pattern) 
  : pattern(pattern) {
  try {
    regexp = std::regex(pattern, std::regex_constants::extended);
  } catch (std::regex_error &e) {
    handle_regex_error(e);
  }
}

std::string Pattern::toString() const {
  return this->pattern;
}

bool Pattern::instanceof(const std::type_info &info) const {
  return (
          typeid(Pattern) == info
          );
}
