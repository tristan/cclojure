#ifndef _ratio_h
#define _ratio_h

#include <unicode/ustdio.h>
#include "string.h"
#include "number.h"
#include "integer.h"

#define _extend_Ratio \
  _extend_Number; \
  Integer *(*numerator)(const void *self); \
  Integer *(*denominator)(const void *self); \
  void *_num

struct Ratio {
  _extend_Ratio;
};

typedef struct Ratio Ratio;

Ratio *Ratio_new_I(const Integer* numerator, const Integer* denominator);
Ratio *Ratio_new_i(int numerator, int denominator);
Ratio *Ratio_new_u(const UChar *numerator, const UChar *denominator);
Ratio *Ratio_new_s(const String *numerator, const String *denominator);

#endif
