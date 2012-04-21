#ifndef _BOOLEAN_H
#define _BOOLEAN_H

// boolean wrapper class
class boolean : public object {
public:
  boolean(bool b);
  bool operator==(const object& o) const override;
  std::string to_string() const override;
protected:
  bool value;
};

#endif
