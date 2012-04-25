#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include "clojure.h"

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

size_t Integer::hashCode() const {
  return this->value;
}

int Integer::compareTo(const Comparable& o) const {
  if (typeid(o) != typeid(Integer)) {
    throw std::string(typeid(o).name()) + " cannot be cast to Integer";
  }
  const Integer& i = dynamic_cast<const Integer&>(o);
  if (this->value < i.value) {
    return -1;
  }
  if (this->value > i.value) {
    return 1;
  }
  return 0;
}

bool Integer::instanceof(const std::type_info &info) const {
  return (
          typeid(Integer) == info ||
          typeid(Number) == info ||
          typeid(Comparable) == info
          );
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

size_t Irrational::hashCode() const {
  // taken from openjdk's Double.hashCode implementation
  union {
    long l;
    double d;
  } u;
  u.d = this->value;

  long bits = u.l;
  if ( ((bits & 0x7FF0000000000000L) ==
        0x7FF0000000000000L) &&
       (bits & 0x000FFFFFFFFFFFFFL) != 0L) {
    bits = 0x7ff8000000000000L;
  }
  int r = (int)(bits ^ (bits >> 32));
  return r;
}

int Irrational::compareTo(const Comparable& o) const {
  if (typeid(o) != typeid(Irrational)) {
    throw std::string(typeid(o).name()) + " cannot be cast to Irrational";
  }
  const Irrational& i = dynamic_cast<const Irrational&>(o);
  if (this->value < i.value) {
    return -1;
  }
  if (this->value > i.value) {
    return 1;
  }
  return 0;
}

bool Irrational::instanceof(const std::type_info &info) const {
  return (
          typeid(Irrational) == info ||
          typeid(Number) == info ||
          typeid(Comparable) == info
          );
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

size_t Ratio::hashCode() const {
  return this->num->hashCode() ^ this->den->hashCode();
}

int Ratio::compareTo(const Comparable& o) const {
  if (typeid(o) != typeid(Ratio)) {
    throw std::string(typeid(o).name()) + " cannot be cast to Ratio";
  }
  const Ratio& r = dynamic_cast<const Ratio&>(o);
  double tv = (1.0 * this->num->value) / this->den->value;
  double rv = (1.0 * r.num->value) / r.den->value;
  if (tv < rv) {
    return -1;
  }
  if (tv > rv) {
    return 1;
  }
  return 0;
}

bool Ratio::instanceof(const std::type_info &info) const {
  return (
          typeid(Ratio) == info ||
          typeid(Number) == info ||
          typeid(Comparable) == info
          );
}
