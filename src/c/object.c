#include <stdlib.h>
#include <unicode/ustdio.h>
#include <unicode/ustring.h>
#include "object.h"

void Object_destroy(void *self) {
  free(self);
}

UChar *Object_toString(void *self) {
  UChar *str = malloc(sizeof(UChar) * 16); // address should be 8 chars
  u_sprintf(str, "Object@%x", self);
  return str;
}

int Object_getClass(void *self) {
  return OBJECT_CLASS;
}

// TODO: this is wrong, and needs to be re-thought
// (((Object*)self)->getClass(self) == class
int Object_instanceOf(void *self, int class) {
  return class == OBJECT_CLASS;
}

int Object_equals(void *self, void *obj) {
  // this should never be called, but if it does, return false
  return 0;
}
