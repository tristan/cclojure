#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include "clojure.h"

/*std::string Number::toString() const {
  return "#<Number Object>";
  }*/

Integer::Integer(const std::string &i) {
  // TODO: support larger ints
  this->value = std::stoll(i);
}

Integer::Integer(const std::string &i, int base) {
  // TODO: support larger ints
  this->value = std::stoll(i, 0, base);
}

Integer::Integer(long long i) {
  this->value = i;
}

std::string Integer::toString() const {
  // TODO: it may be quicker to use sprintf here
  std::stringstream sstm;
  sstm << value;
  return sstm.str();
}

Irrational::Irrational(const std::string &i) {
  this->value = std::stod(i);
  /* TODO: the output of doubles is different from clojure
     perhaps this matters
  size_t e = i.rfind('e');
  if (e == std::string::npos) {
    e = i.rfind('E');
  }
  if (e == std::string::npos) {
    e = i.find('.');
    if (e != std::string::npos) {
      precision = (i.size() - e) - 1;
      if (precision > 6) {
        precision = 6;
      }
    }
  } else {
    if (i.at(e) == '-') {
      precision = std::stoi(i.substr(e+1));
    }
  }
  std::cout << "set precision to: " << precision << std::endl;
  */
}

Irrational::Irrational(double i) {
  this->value = i;
}

std::string Irrational::toString() const {
  // TODO: it may be quicker to use sprintf here
  std::stringstream sstm;
  if (precision > 0) {
    sstm.precision(precision);
    sstm << std::fixed << value;
  } else {
    sstm << value;
  }
  return sstm.str();
}

Ratio::Ratio(const std::string &num, const std::string &den) {
  this->num = make_unique<Integer>(num);
  this->den = make_unique<Integer>(den);
}

Ratio::Ratio(long long num, long long den) {
  this->num = make_unique<Integer>(num);
  this->den = make_unique<Integer>(den);
}

Ratio::Ratio(const Integer &num, const Integer &den) {
  this->num = make_unique<Integer>(num);
  this->den = make_unique<Integer>(den);
}

std::string Ratio::toString() const {
  std::stringstream sstm;
  sstm << num->toString() << "/" << den->toString();
  return sstm.str();
}
