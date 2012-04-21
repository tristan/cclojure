#ifndef _BOOLEAN_H
#define _BOOLEAN_H

// boolean wrapper class
class Boolean : public Object {
public:
  Boolean(bool b);
  bool operator==(const Object& o) const override;
  std::string toString() const override;
protected:
  bool value;
};

#endif
