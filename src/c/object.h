#ifndef _object_h
#define _object_h

#define _DEBUG 1

#include <stdlib.h>
#include <unicode/uchar.h>
#include "classes.h"

#define _extend_Object \
  void (*destroy)(void *self); \
  UChar *(*toString)(const void *self); \
  int (*getClass)(const void *self); \
  int (*instanceOf)(const void *self, int class); \
  int (*equals)(const void *self, const void *obj); \
  void *(*clone)(const void *self); \
  int __refcount

struct Object;
typedef struct Object Object;

struct Object {
  _extend_Object;
};

void Object_destroy(void *self);
UChar *Object_toString(const void *self);
int Object_getClass(const void *self);
int Object_instanceOf(const void *self, int class);
int Object_equals(const void *self, const void *obj);
void *Object_clone(const void *self);

// TODO: calloc success check
#define _super_Object_new(T, O) \
  if (!T##Proto.destroy) T##Proto.destroy = Object_destroy; \
  if (!T##Proto.toString) T##Proto.toString = Object_toString; \
  if (!T##Proto.getClass) T##Proto.getClass = Object_getClass; \
  if (!T##Proto.instanceOf) T##Proto.instanceOf = Object_instanceOf; \
  if (!T##Proto.equals) T##Proto.equals = Object_equals; \
  if (!T##Proto.clone) T##Proto.clone = Object_clone; \
  T *O = calloc(1, sizeof(T)); \
  *((Object*)O) = T##Proto; \
  O->__refcount = 1
//}

#define add_ref(O) (O)->__refcount++
#define drop_ref(O) if ((O) != NULL && !(--((O)->__refcount))) (O)->destroy((O))

#endif
