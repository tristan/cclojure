#include "reader.h"

UChar *Reader_toString(void *self) {
  UChar *str = malloc(sizeof(UChar) * 16); // address should be 8 chars
  u_sprintf(str, "Reader@%x", self);
  return str;
}

int Reader_getClass(void *self) {
  return READER_CLASS;
}

int Reader_instanceOf(void *self, int class) {
  return class == READER_CLASS || Object_instanceOf(self, class);
}
