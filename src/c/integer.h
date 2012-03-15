#ifndef _integer_h
#define _integer_h

#include <unicode/ustdio.h>
#include "number.h"

#define _extend_Integer \
  _extend_Number \
  void *_num;

struct Integer {
  _extend_Integer
};

typedef struct Integer Integer;

Integer *Integer_new();
Integer *Integer_valueOf(const UChar *s, int radix);

#endif
