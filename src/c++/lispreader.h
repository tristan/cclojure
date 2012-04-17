#ifndef _LISPREADER_H
#define _LISPREADER_H

class lispreader {
 public:
  object &read(std::istream &in, bool eof_is_error, object &eof_value, bool is_recursive);
};

#endif
