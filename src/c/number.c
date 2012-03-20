#include "number.h"

UChar *Number_toString(const void *self) {
  UChar *str = malloc(sizeof(UChar) * 16); // address should be 8 chars
  u_sprintf(str, "Number@%x", self);
  return str;
}

int Number_getClass(const void *self) {
  return NUMBER_CLASS;
}

int Number_instanceOf(const void *self, int class) {
  return class == NUMBER_CLASS || Object_instanceOf(self, class);
}

void *Number_negate(void *self) {
  puts("negate not supported by this number type");
  return self;
}
