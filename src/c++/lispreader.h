#ifndef _LISPREADER_H
#define _LISPREADER_H

class LispReader {
 public:
  // the lispReader doesn't need to store any state so it can be static
  static std::shared_ptr<Object> read(std::istream &in, bool eof_is_error, 
                                      std::shared_ptr<Object> eof_value, bool is_recursive);
};

#endif
