#ifndef _STRING_H
#define _STRING_H

// String wrapper class
class String : public Object {
public:
  String(const std::string& s);
  String(const String& s);
  bool operator==(const Object &o) const override;

  // TODO: is there an operator of some kind that can handle
  // automatically calling toString if this is passed
  // to a function instead of an std::string

  std::string toString() const override;
  size_t hashCode() const override;

  int compareTo(const Object& o) const override;

protected:
  // TODO: would making this const make it immutatable?
  // if i really want an immutable String should i just use c_str?
  std::string str;
};

#endif
