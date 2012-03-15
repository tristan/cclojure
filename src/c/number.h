#ifndef _number_h
#define _number_h

#include <unicode/ustdio.h>
#include "object.h"

#define _extend_Number \
  _extend_Object \

struct Number {
  _extend_Number
};

typedef struct Number Number;

UChar *Number_toString(void *reader);
int Number_getClass(void *reader);
int Number_instanceOf(void *reader, int class);

#define _super_Number_new(T, O) \
  if (!T##Proto.init) return NULL; \
  if (!T##Proto.destroy) T##Proto.destroy = Object_destroy; \
  if (!T##Proto.toString) T##Proto.toString = Number_toString; \
  if (!T##Proto.getClass) T##Proto.getClass = Number_getClass; \
  if (!T##Proto.instanceOf) T##Proto.instanceOf = Number_instanceOf; \
  T *O = calloc(1, sizeof(T));                          \
  *((Number*)O) = T##Proto

#endif
