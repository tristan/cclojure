#ifndef _LISPREADER_H
#define _LISPREADER_H

class LispReader {
 public:
  std::shared_ptr<Object> read(std::istream &in, bool eof_is_error, 
                               std::shared_ptr<Object> eof_value, bool is_recursive);
};

#endif
