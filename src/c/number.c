#include "number.h"

UChar *Number_toString(void *self) {
  UChar *str = malloc(sizeof(UChar) * 16); // address should be 8 chars
  u_sprintf(str, "Number@%x", self);
  return str;
}

int Number_getClass(void *self) {
  return NUMBER_CLASS;
}

int Number_instanceOf(void *self, int class) {
  return class == NUMBER_CLASS || Object_instanceOf(self, class);
}

