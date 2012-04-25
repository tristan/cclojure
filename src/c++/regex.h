#ifndef _CLJ_REGEX_H
#define _CLJ_REGEX_H

#include <string>
#include <regex>

// TODO: either i need to learn how to use the stl regex to properly mimic the same
// behaviour of java regex, or i need to find a better library (or i could re-impl the
// openjdk impl in c++... but that seems a bit extreme, and time consuming!)
class Pattern : public Object {
public:
  Pattern(const std::string &pattern);
  std::string toString() const override;
  bool instanceof(const std::type_info &info) const override;
private:
  std::string pattern;
  std::regex regexp;
};

#endif
