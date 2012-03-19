#include "pattern.h"
#include <unicode/ustring.h>
#include <unicode/uregex.h>
#include <string.h>

#define _r(N) (URegularExpression*)((Pattern*)N)->_state

void Pattern_destroy(void *self) {
  if (_r(self)) {
    uregex_close(_r(self));
  }
  free(self);
}

UChar *Pattern_toString(void *self) {
  UErrorCode status = 0;
  const UChar *s = uregex_pattern(_r(self), NULL, &status);
  UChar *r = malloc(sizeof(UChar) * (u_strlen(s) + 1));
  u_strcpy(r, s);
  return r;
}

int Pattern_getClass(void *self) {
  return PATTERN_CLASS;
}

int Pattern_instanceOf(void *self, int class) {
  return class == PATTERN_CLASS || Object_instanceOf(self, class);
}

Matcher *Pattern_matcher_u(void *self, const UChar *s) {
  return Matcher_new_u((Pattern*)self, s);
}

Matcher *Pattern_matcher_a(void *self, const char *s) {
  return Matcher_new_a((Pattern*)self, s);
}

Matcher *Pattern_matcher_s(void *self, String *s) {
  return Matcher_new_s((Pattern*)self, s);
}

void *Pattern_clone(void *self) {
  UErrorCode status = 0;
  const UChar *s = uregex_pattern(_r(self), NULL, &status);
  return (void*)Pattern_compile_u(s, 0);
}

Object PatternProto = {
  .getClass = Pattern_getClass,
  .instanceOf = Pattern_instanceOf,
  .toString = Pattern_toString,
  .destroy = Pattern_destroy,
  .clone = Pattern_clone
};

Pattern *Pattern_compile_u(const UChar *str, int flags) {
  _super_Object_new(Pattern, obj);

  UErrorCode status = 0;
  UParseError pe;
  URegularExpression *re = uregex_open(str,
					-1,
                    0, // TODO: handle flags
					&pe,
					&status);

  if (status != 0) {
    printf("ERROR compiling Regular Expression: %s\n", (char *)pe.preContext);
    free(obj);
    return NULL;
  }

  obj->matcher_u = Pattern_matcher_u;
  obj->matcher_a = Pattern_matcher_a;
  obj->matcher_s = Pattern_matcher_s;
  obj->_state = (void*)re;
  return obj;
}

Pattern *Pattern_compile_a(const char *str, int flags) {
  UChar *s = malloc(strlen(str)+1);
  u_uastrcpy(s, str);
  Pattern *p = Pattern_compile_u(s, flags);
  free(s);
  return p;  
}

Pattern *Pattern_compile_s(String *str, int flags) {
  UChar *s = str->toString(str);
  Pattern *p = Pattern_compile_u(s, flags);
  free(s);
  return p;
}
