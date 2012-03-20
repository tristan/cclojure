#ifndef _matcher_h
#define _matcher_h

#include <unicode/ustdio.h>
#include "string.h"
#include "object.h"

typedef struct Pattern Pattern;

#define _extend_Matcher \
  _extend_Object; \
  String *(*group)(const void *self, int group); \
  int (*groupCount)(const void *self); \
  int (*matches)(void *self); \
  Pattern *_pat

struct Matcher {
  _extend_Matcher;
};

typedef struct Matcher Matcher;

Matcher *Matcher_new_u(const Pattern *p, const UChar *s);
Matcher *Matcher_new_a(const Pattern *p, const char *s);
Matcher *Matcher_new_s(const Pattern *p, const String *s);

#endif
