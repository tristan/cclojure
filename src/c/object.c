#include <stdlib.h>
#include <unicode/ustdio.h>
#include <unicode/ustring.h>
#include "object.h"

static int class_count = 0;

int register_new_class() {
  return ++class_count;
}

void Object_destroy(void *self) {
  free(self);
}

String *Object_toString(void *self) {
  UChar *str = malloc(sizeof(UChar) * 16); // address should be 8 chars
  u_sprintf(str, "Object@%x", self);
  return str;
}

int Object_getClass(void *self) {
  return OBJECT_CLASS;
}

int Object_instanceOf(void *self, int class) {
  return class == OBJECT_CLASS;
}
