#include <stdio.h>
#include <string.h>
#include <unicode/ustring.h>
#include "string.h"

#define _s(N) ((String*)N)->_str

void String_destroy(void *self) {
  if (_s(self)) {
    free(_s(self));
  }
  free(self);
}

UChar *String_toString(void *self) {
  UChar *s = _s(self);
  UChar *r = malloc(sizeof(UChar) * (u_strlen(s) + 1));
  u_strcpy(r, s);
  return r;
}

int String_getClass(void *self) {
  return STRING_CLASS;
}

int String_instanceOf(void *self, int class) {
  return class == STRING_CLASS || Object_instanceOf(self, class);
}

int String_equals(void *self, void *obj) {
  if (((String*)self)->getClass(self) != String_getClass(self)) {
    puts("Calling String_equals on non string object is strange!");
  } else if (((String*)obj)->getClass(obj) == String_getClass(self)) {
    return (u_strcmp(_s(self), _s(obj)) == 0);
  }
  return 0;
}

Object StringProto = {
  .destroy = String_destroy,
  .getClass = String_getClass,
  .toString = String_toString,
  .instanceOf = String_instanceOf,
  .equals = String_equals
};

String* String_new_u(const UChar *str) {
  _super_Object_new(String, obj);
  UChar *s = malloc(sizeof(UChar) * (u_strlen(str) + 1));
  if (!s) {
    puts("Unable to allocate memory for new String");
    return NULL;
  }
  u_strcpy(s, str);
  obj->_str = s;
  return obj;
}

String *String_new_a(const char *str) {
  _super_Object_new(String, obj);
  UChar *s = malloc(sizeof(UChar) * (strlen(str) + 1));
  if (!s) {
    puts("Unable to allocate memory for new String");
    return NULL;
  }
  u_uastrcpy(s, str);
  obj->_str = s;
  return obj;
}
