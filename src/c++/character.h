#ifndef _CLJ_CHARACTER_H
#define _CLJ_CHARACTER_H

// boolean wrapper class
class Character : public Object, public Comparable {
public:
  Character(long i);
  bool operator==(const Object& o) const override;
  std::string toString() const override;
  int compareTo(const Comparable& o) const override;
  bool instanceof(const std::type_info &info) const override;
protected:
  long ch;
};

#endif
