#include <stdlib.h>
#include <unicode/ustdio.h>
#include <unicode/ustring.h>
#include "object.h"

void Object_destroy(void *self) {
#ifdef _DEBUG
  puts("Called Object_destroy");
#endif
  free(self);
}

UChar *Object_toString(const void *self) {
  UChar *str = malloc(sizeof(UChar) * 16); // address should be 8 chars
  u_sprintf(str, "Object@%x", self);
  return str;
}

int Object_getClass(const void *self) {
  return OBJECT_CLASS;
}

// TODO: this is wrong, and needs to be re-thought
// (((Object*)self)->getClass(self) == class
int Object_instanceOf(const void *self, int class) {
  return class == OBJECT_CLASS;
}

int Object_equals(const void *self, const void *obj) {
  // this should never be called, but if it does, return false
  return 0;
}

void *Object_clone(const void *self) {
  puts("clone not supported");
  return NULL;
}
