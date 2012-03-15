#ifndef _numbers_h
#define _numbers_h

#include <unicode/ustring.h>

enum {
  INTEGER,
  FLOAT,
  RATIO
};

struct Number {
  int type;
  void *data;
};

typedef struct Number Number;


Number *Integer_init_int(int i);
//Number *Integer_init_str(const char *str);
Number *Integer_init_str(const UChar *str, int base);
Number *Float_init_f(float f);
Number *Float_init_str(const UChar *str);
Number *Ratio_init_str(const UChar *snum, const UChar *sden);

void Number_destroy(Number *num);

UChar *Number_toString(Number *num);

void Integer_negate(Number *num);

#endif
