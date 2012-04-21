#ifndef _STRING_H
#define _STRING_H

// string wrapper class
class string : public object {
public:
  string(const std::string& s);
  string(const string& s);
  bool operator==(const object &o) const override;
  
  std::string to_string() const override;
  size_t hash_code() const override;
protected:
  // TODO: would making this const make it immutatable?
  // if i really want an immutable string should i just use c_str?
  std::string str;
};

#endif
