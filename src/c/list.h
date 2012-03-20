#ifndef _list_h
#define _list_h

#include <unicode/ustdio.h>
#include "object.h"

#define _extend_List \
  _extend_Object; \
  int (*add)(void *self, Object *e); \
  int (*add_at)(void *self, int index, Object *e);  \
  void (*clear)(void *self); \
  int (*contains)(const void *self, const Object *o); \
  Object *(*get)(const void *self, int index); \
  int (*indexOf)(const void *self, const Object *o); \
  int (*isEmpty)(const void *self); \
  Object *(*remove)(void *self, int index); \
  int (*remove_object)(void *self, const Object *o); \
  Object *(*set)(void *self, int index, Object *element); \
  int (*size)(const void *self); \
  Object **(*toArray)(const void *self)

struct List {
  _extend_List;
};

typedef struct List List;

UChar *List_toString(const void *self);
int List_getClass(const void *self);
int List_instanceOf(const void *self, int class);
void List_destroy(void *self);
int List_equals(const void *self, const void *list);
Object **List_toArray(const void *self);
int List_indexOf(const void *self, const Object *o);
int List_remove_object(void *self, const Object *o);
int List_contains(const void *self, const Object *o);
int List_isEmpty(const void *self);

#define _super_List_new(T, O) \
  if (!T##Proto.destroy) T##Proto.destroy = List_destroy; \
  if (!T##Proto.toString) T##Proto.toString = List_toString; \
  if (!T##Proto.getClass) T##Proto.getClass = List_getClass; \
  if (!T##Proto.instanceOf) T##Proto.instanceOf = List_instanceOf; \
  if (!T##Proto.equals) T##Proto.equals = List_equals; \
  if (!T##Proto.toArray) T##Proto.toArray = List_toArray; \
  if (!T##Proto.isEmpty) T##Proto.isEmpty = List_isEmpty; \
  if (!T##Proto.contains) T##Proto.contains = List_contains; \
  if (!T##Proto.indexOf) T##Proto.indexOf = List_indexOf; \
  if (!T##Proto.remove_object) T##Proto.remove_object = List_remove_object; \
  if (!T##Proto.clone) T##Proto.clone = Object_clone; \
  T *O = calloc(1, sizeof(T)); \
  *((List*)O) = T##Proto; \
  O->__refcount = 1

#endif
