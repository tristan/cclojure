#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include "clojure.h"

std::string number::to_string() {
  return "#<number object>";
}

integer::integer(std::string i) {
  // TODO: support larger ints
  this->value = std::stoll(i);
}

integer::integer(std::string i, int base) {
  // TODO: support larger ints
  this->value = std::stoll(i, 0, base);
}

integer::integer(long long i) {
  this->value = i;
}

std::string integer::to_string() {
  // TODO: it may be quicker to use sprintf here
  std::stringstream sstm;
  sstm << value;
  return sstm.str();
}

irrational::irrational(std::string i) {
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

irrational::irrational(double i) {
  this->value = i;
}

std::string irrational::to_string() {
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

ratio::ratio(std::string num, std::string den) {
  this->num = make_unique<integer>(num);
  this->den = make_unique<integer>(den);
}

ratio::ratio(long long num, long long den) {
  this->num = make_unique<integer>(num);
  this->den = make_unique<integer>(den);
}

ratio::ratio(integer &num, integer &den) {
  this->num = make_unique<integer>(num);
  this->den = make_unique<integer>(den);
}

std::string ratio::to_string() {
  std::stringstream sstm;
  sstm << num->to_string() << "/" << den->to_string();
  return sstm.str();
}
