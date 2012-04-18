#ifndef _LISPREADER_H
#define _LISPREADER_H

class lispreader {
 public:
  std::shared_ptr<object> read(std::istream &in, bool eof_is_error, 
                               std::shared_ptr<object> eof_value, bool is_recursive);
};

#endif
