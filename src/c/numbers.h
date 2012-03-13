#ifndef _numbers_h
#define _numbers_h

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
Number *Integer_init_str(const char *str, int base);
Number *Float_init_f(float f);
Number *Float_init_str(const char *str);
Number *Ratio_init_str(const char *snum, const char *sden);

void Number_destroy(Number *num);

char *Number_to_str(Number *num);

#endif
