#ifndef _NUMBERS_H
#define _NUMBERS_H

class Number : public Object, public Comparable {
  //std::string toString() const override;
protected:
  Number() {};
};

class Integer : public Number {
public:
  Integer(const std::string &i);
  Integer(const std::string &i, int base);
  Integer(long long i);

  std::string toString() const override;
  size_t hashCode() const override;
  int compareTo(const Comparable& o) const override;

  bool instanceof(const std::type_info &info) const override;
  friend class Ratio;
private:
  long long value;
};

class Irrational : public Number {
public:
  Irrational(const std::string &i);
  Irrational(double i);

  std::string toString() const override;
  size_t hashCode() const override;
  int compareTo(const Comparable& o) const override;

  bool instanceof(const std::type_info &info) const override;
private:
  double value;
  int precision = -1;
};

class Ratio : public Number {
public:
  Ratio(const std::string &num, const std::string &den);
  Ratio(long long num, long long den);
  Ratio(const Integer &num, const Integer &den);

  std::string toString() const override;
  size_t hashCode() const override;
  int compareTo(const Comparable& o) const override;

  bool instanceof(const std::type_info &info) const override;
private:
  std::unique_ptr<Integer> num;
  std::unique_ptr<Integer> den;
};

#endif
