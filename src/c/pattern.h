#ifndef _pattern_h
#define _pattern_h

#include <unicode/ustdio.h>
#include "string.h"
#include "matcher.h"
#include "object.h"

#define _extend_Pattern \
  _extend_Object; \
  Matcher *(*matcher_u)(const void *self, const UChar *s); \
  Matcher *(*matcher_a)(const void *self, const char *s); \
  Matcher *(*matcher_s)(const void *self, const String *s); \
  void *_state

struct Pattern {
  _extend_Pattern;
};

typedef struct Pattern Pattern;

Pattern *Pattern_compile_u(const UChar *str, int flags);
Pattern *Pattern_compile_a(const char *str, int flags);
Pattern *Pattern_compile_s(const String *str, int flags);

#endif
