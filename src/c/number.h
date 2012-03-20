#ifndef _number_h
#define _number_h

#include <unicode/ustdio.h>
#include "object.h"

#define _extend_Number \
  _extend_Object; \
  void *(*negate)(void *self)

struct Number {
  _extend_Number;
};

typedef struct Number Number;

UChar *Number_toString(const void *self);
int Number_getClass(const void *self);
int Number_instanceOf(const void *self, int class);

void *Number_negate(void *self);

#define _super_Number_new(T, O) \
  if (!T##Proto.destroy) T##Proto.destroy = Object_destroy; \
  if (!T##Proto.toString) T##Proto.toString = Number_toString; \
  if (!T##Proto.getClass) T##Proto.getClass = Number_getClass; \
  if (!T##Proto.instanceOf) T##Proto.instanceOf = Number_instanceOf; \
  if (!T##Proto.equals) T##Proto.equals = Object_equals; \
  if (!T##Proto.negate) T##Proto.negate = Number_negate; \
  if (!T##Proto.clone) T##Proto.clone = Object_clone; \
  T *O = calloc(1, sizeof(T)); \
  *((Number*)O) = T##Proto; \
  O->__refcount = 1

#endif
