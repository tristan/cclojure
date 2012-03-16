#ifndef _decimal_h
#define _decimal_h

#include <unicode/ustdio.h>
#include "number.h"

#define _extend_Decimal \
  _extend_Number; \
  void *_num

struct Decimal {
  _extend_Decimal;
};

typedef struct Decimal Decimal;

Decimal *Decimal_new(double num);
Decimal *Decimal_valueOf(const UChar *s);

#endif
