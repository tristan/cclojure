#ifndef _matcher_h
#define _matcher_h

#include <unicode/ustdio.h>
#include "string.h"
#include "object.h"

typedef struct Pattern Pattern;

#define _extend_Matcher \
  _extend_Object; \
  String *(*group)(void *self, int group); \
  int (*groupCount)(void *self); \
  int (*matches)(void *self); \
  Pattern *_pat

struct Matcher {
  _extend_Matcher;
};

typedef struct Matcher Matcher;

Matcher *Matcher_new_u(Pattern *p, const UChar *s);
Matcher *Matcher_new_a(Pattern *p, const char *s);
Matcher *Matcher_new_s(Pattern *p, String *s);

#endif
