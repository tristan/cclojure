#ifndef _string_h
#define _string_h

#include <unicode/ustdio.h>
#include "object.h"

#define _extend_String \
  _extend_Object; \
  UChar (*charAt)(const void *self, int index); \
  int (*length)(const void *self); \
  UChar *_str

struct String {
  _extend_String;
};

typedef struct String String;

String *String_new_u(const UChar *str);
String *String_new_a(const char *str);

#endif
