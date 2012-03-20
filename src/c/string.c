#include <stdio.h>
#include <string.h>
#include <unicode/ustring.h>
#include "string.h"
#include "unicode_utils.h"

#define _s(N) ((String*)N)->_str

void String_destroy(void *self) {
#ifdef _DEBUG
  u_printf("destroying String \"%S\"\n", _s(self));
#endif
  if (_s(self)) {
    free(_s(self));
  }
  free(self);
}

UChar *String_toString(const void *self) {
  UChar *s = _s(self);
  UChar *r = malloc(sizeof(UChar) * (u_strlen(s) + 1));
  u_strcpy(r, s);
  return r;
}

int String_getClass(const void *self) {
  return STRING_CLASS;
}

int String_instanceOf(const void *self, int class) {
  return class == STRING_CLASS || Object_instanceOf(self, class);
}

int String_equals(const void *self, const void *obj) {
  if (((String*)self)->getClass(self) != String_getClass(self)) {
    puts("Calling String_equals on non string object is strange!");
  } else if (((String*)obj)->getClass(obj) == String_getClass(self)) {
    return (u_strcmp(_s(self), _s(obj)) == 0);
  }
  return 0;
}

int String_length(const void *self) {
  return u_strlen(((String*)self)->_str);
}

UChar String_charAt(const void *self, int index) {
  if (index < 0 || index >= ((String*)self)->length(self)) {
    return -1;
  }
  return ((String*)self)->_str[index];
}

Object StringProto = {
  .destroy = String_destroy,
  .getClass = String_getClass,
  .toString = String_toString,
  .instanceOf = String_instanceOf,
  .equals = String_equals
};

#define String_new_global(X) \
  _super_Object_new(String, X); \
  X->charAt = String_charAt; \
  X->length = String_length


String* String_new_u(const UChar *str) {
  String_new_global(obj);
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
  String_new_global(obj);
  UChar *s = malloc(sizeof(UChar) * (strlen(str) + 1));
  if (!s) {
    puts("Unable to allocate memory for new String");
    return NULL;
  }
  u_uastrcpy(s, str);
  obj->_str = s;
  return obj;
}
