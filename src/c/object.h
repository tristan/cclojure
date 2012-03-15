#ifndef _object_h
#define _object_h

#include <stdlib.h>
#include <unicode/uchar.h>
#include "classes.h"

typedef UChar String;

#define _extend_Object \
  int (*init)(void *self, void* args); \
  void (*destroy)(void *self); \
  String *(*toString)(void *self); \
  int (*getClass)(void *self); \
  int (*instanceOf)(void *self, int class);

struct Object;
typedef struct Object Object;

struct Object {
  _extend_Object
};

void Object_destroy(void *self);
String *Object_toString(void *self);
int Object_getClass(void *self);
int Object_instanceOf(void *self, int class);

// call this to get a new id to store the value of a class type
int register_new_class();

//Object *Object_new(size_t size, Object proto, ...) {
#define _super_Object_new(T, O)      \
  if (!T##Proto.init) return NULL; \
  if (!T##Proto.destroy) T##Proto.destroy = Object_destroy; \
  if (!T##Proto.toString) T##Proto.toString = Object_toString; \
  if (!T##Proto.getClass) T##Proto.getClass = Object_getClass; \
  if (!T##Proto.instanceOf) T##Proto.instanceOf = Object_instanceOf; \
  T *O = calloc(1, sizeof(T));                          \
  *((Object*)O) = T##Proto
//}

#endif
