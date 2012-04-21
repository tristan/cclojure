#ifndef _NUMBERS_H
#define _NUMBERS_H

class number : public object {
  //std::string to_string() const override;
};

class integer : public number {
public:
  integer(const std::string &i);
  integer(const std::string &i, int base);
  integer(long long i);

  std::string to_string() const override;

  friend class ratio;
private:
  long long value;
};

class irrational : public number {
public:
  irrational(const std::string &i);
  irrational(double i);

  std::string to_string() const override;
private:
  double value;
  int precision = -1;
};

class ratio : public number {
public:
  ratio(const std::string &num, const std::string &den);
  ratio(long long num, long long den);
  ratio(const integer &num, const integer &den);

  std::string to_string() const override;
private:
  std::unique_ptr<integer> num;
  std::unique_ptr<integer> den;
};

#endif
