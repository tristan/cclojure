#ifndef _ratio_h
#define _ratio_h

#include <unicode/ustdio.h>
#include "number.h"
#include "integer.h"

#define _extend_Ratio \
  _extend_Number \
  Integer *(*numerator)(void *self); \
  Integer *(*denominator)(void *self); \
  void *_num;

struct Ratio {
  _extend_Ratio
};

typedef struct Ratio Ratio;

Ratio *Ratio_new();

#endif
