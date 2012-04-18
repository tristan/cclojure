#ifndef _NUMBERS_H
#define _NUMBERS_H

class number : public object {
protected:
  std::string to_string() override;
};

class integer : public number {
public:
  integer(std::string i);
  integer(std::string i, int base);
  integer(long long i);
  friend class ratio;
protected:
  long long value;
  std::string to_string() override;
};

class irrational : public number {
public:
  irrational(std::string i);
  irrational(double i);
protected:
  double value;
  int precision = -1;
  std::string to_string() override;
};

class ratio : public number {
public:
  ratio(std::string num, std::string den);
  ratio(long long num, long long den);
  ratio(integer &num, integer &den);
protected:
  std::unique_ptr<integer> num;
  std::unique_ptr<integer> den;
  std::string to_string() override;
};

#endif
