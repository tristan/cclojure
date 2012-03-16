#ifndef _reader_h
#define _reader_h

#include <unicode/ustdio.h>
#include "object.h"

#define _extend_Reader \
  _extend_Object; \
  UChar (*read)(void *self); \
  UChar (*unread)(void *self, UChar ch); \
  void (*reset)(void *self); \
  long (*skip)(void *self, long n)

struct Reader {
  _extend_Reader;
};

typedef struct Reader Reader;

UChar *Reader_toString(void *reader);
int Reader_getClass(void *reader);
int Reader_instanceOf(void *reader, int class);

#define _super_Reader_new(T, O) \
  if (!T##Proto.read) return NULL; \
  if (!T##Proto.unread) return NULL; \
  if (!T##Proto.reset) return NULL; \
  if (!T##Proto.skip) return NULL; \
  if (!T##Proto.destroy) T##Proto.destroy = Object_destroy; \
  if (!T##Proto.toString) T##Proto.toString = Reader_toString; \
  if (!T##Proto.getClass) T##Proto.getClass = Reader_getClass; \
  if (!T##Proto.instanceOf) T##Proto.instanceOf = Reader_instanceOf; \
  if (!T##Proto.equals) T##Proto.equals = Object_equals; \
  T *O = calloc(1, sizeof(T)); \
  *((Reader*)O) = T##Proto

#endif
