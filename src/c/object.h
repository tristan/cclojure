#ifndef _object_h
#define _object_h

#define _DEBUG 1

#include <stdlib.h>
#include <unicode/uchar.h>
#include "classes.h"

#define _extend_Object \
  void (*destroy)(void *self); \
  UChar *(*toString)(void *self); \
  int (*getClass)(void *self); \
  int (*instanceOf)(void *self, int class); \
  int (*equals)(void *self, void *obj)

struct Object;
typedef struct Object Object;

struct Object {
  _extend_Object;
};

void Object_destroy(void *self);
UChar *Object_toString(void *self);
int Object_getClass(void *self);
int Object_instanceOf(void *self, int class);
int Object_equals(void *self, void *obj);

// TODO: calloc success check
#define _super_Object_new(T, O) \
  if (!T##Proto.destroy) T##Proto.destroy = Object_destroy; \
  if (!T##Proto.toString) T##Proto.toString = Object_toString; \
  if (!T##Proto.getClass) T##Proto.getClass = Object_getClass; \
  if (!T##Proto.instanceOf) T##Proto.instanceOf = Object_instanceOf; \
  if (!T##Proto.equals) T##Proto.equals = Object_equals; \
  T *O = calloc(1, sizeof(T)); \
  *((Object*)O) = T##Proto
//}

#endif
