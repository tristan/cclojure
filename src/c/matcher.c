#include <unicode/ustring.h>
#include <unicode/uregex.h>
#include <string.h>
#include "string_buffer.h"
#include "pattern.h"
#include "matcher.h"

void Matcher_destroy(void *self) {
  if ( ((Matcher*)self)->_pat ) {
    drop_ref( (Pattern*)((Matcher*)self)->_pat );
  }
  free(self);
}

UChar *Matcher_toString(const void *self) {
  UErrorCode status = 0;
  const UChar *s = uregex_pattern((URegularExpression*)((Matcher*)self)->_pat->_state, NULL, &status);
  const UChar header[] = { '#', '<', 'M', 'a', 't', 'c', 'h', 'e', 'r', ' ', 
                           '[', 'p', 'a', 't', 't', 'e', 'r', 'n', '=', 0 };
  //const UChar sep[] = { ',', ' ', 0 };
  const UChar tail[] = { ']', '>', 0 };
  StringBuffer *sb = StringBuffer_new();
  StringBuffer_append_string(sb, header);
  StringBuffer_append_string(sb, s);
  StringBuffer_append_string(sb, tail);
  UChar *str = StringBuffer_toString(sb);
  StringBuffer_destroy(sb);
  return str;
}

int Matcher_getClass(const void *self) {
  return MATCHER_CLASS;
}

int Matcher_instanceOf(const void *self, int class) {
  return class == MATCHER_CLASS || Object_instanceOf(self, class);
}

String *Matcher_group(const void *self, int group) {
  UChar *resp;
  URegularExpression* regex = (URegularExpression*)((Matcher*)self)->_pat->_state;
  UErrorCode status;
  int s = uregex_start(regex, group, &status);
  int e = uregex_end(regex, group, &status);
  if (s >= 0) {
    int len = (e - s) + 1;
    resp = malloc(sizeof(UChar) * len);
    len = uregex_group(regex, group, resp, len, &status);
    if (len >= 0) {
      return String_new_u(resp);
    }
    puts("unexpectedly failed to get group");
  }
  return NULL;
}

int Matcher_groupCount(const void *self) {
  UErrorCode status = 0;
  return uregex_groupCount((URegularExpression*)((Matcher*)self)->_pat->_state, &status);
}

int Matcher_matches(void *self) {
  UErrorCode status = 0;
  int r = uregex_matches((URegularExpression*)((Matcher*)self)->_pat->_state, -1, &status);
  if (status != 0) {
    puts("Error matching regex");
    return 0;
  } else {
    return r;
  }
}

Object MatcherProto = {
  .getClass = Matcher_getClass,
  .instanceOf = Matcher_instanceOf,
  .toString = Matcher_toString,
  .destroy = Matcher_destroy
};

Matcher *Matcher_new_u(const Pattern *p, const UChar *s) {
  _super_Object_new(Matcher, obj);
  obj->_pat = p->clone(p);

  UErrorCode status = 0;
  uregex_setText((URegularExpression*)obj->_pat->_state, s, -1, &status);
  if (status != 0) {
    puts("Error setting string for regex matcher");
    obj->destroy(obj);
    return NULL;
  }

  obj->group = Matcher_group;
  obj->groupCount = Matcher_groupCount;
  obj->matches = Matcher_matches;
  return obj;
}

Matcher *Matcher_new_a(const Pattern *p, const char *s) {
  UChar *u = malloc(strlen(s)+1);
  u_uastrcpy(u, s);
  Matcher *m = Matcher_new_u(p, u);
  free(u);
  return m;
}

Matcher *Matcher_new_s(const Pattern *p, const String *s) {
  UChar *u = s->toString(s);
  Matcher *m = Matcher_new_u(p, u);
  free(u);
  return m;
}
