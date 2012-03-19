#ifndef _list_h
#define _list_h

#include <unicode/ustdio.h>
#include "object.h"

#define _extend_List \
  _extend_Object; \
  int (*add)(void *self, Object *e); \
  int (*add_at)(void *self, int index, Object *e);  \
  void (*clear)(void *self); \
  int (*contains)(void *self, Object *o); \
  Object *(*get)(void *self, int index); \
  int (*indexOf)(void *self, Object *o); \
  int (*isEmpty)(void *self); \
  Object *(*remove)(void *self, int index); \
  int (*remove_object)(void *self, Object *o); \
  Object *(*set)(void *self, int index, Object *element); \
  int (*size)(void *self); \
  Object **(*toArray)(void *self)

struct List {
  _extend_List;
};

typedef struct List List;

UChar *List_toString(void *self);
int List_getClass(void *self);
int List_instanceOf(void *self, int class);
void List_destroy(void *self);
int List_equals(void *self, void *list);
Object **List_toArray(void *self);
int List_indexOf(void *self, Object *o);
int List_remove_object(void *self, Object *o);
int List_contains(void *self, Object *o);
int List_isEmpty(void *self);

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
  T *O = calloc(1, sizeof(T)); \
  *((List*)O) = T##Proto

#endif
