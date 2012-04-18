#ifndef _LISPREADER_H
#define _LISPREADER_H

class lispreader {
 public:
  obj read(std::istream &in, bool eof_is_error, 
           obj eof_value, bool is_recursive);
};

#endif
