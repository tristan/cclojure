#include <stdlib.h>
#include <stdio.h>
#include "string_buffer.h"

#define DEFAULT_STRING_BUFFER_CAPACITY 32
#define NUMBER_STR_LENGTH 32

StringBuffer *StringBuffer_new() {
  StringBuffer *buf = malloc(sizeof(StringBuffer));
  buf->string = malloc(DEFAULT_STRING_BUFFER_CAPACITY * sizeof(UChar));
  buf->max_len = DEFAULT_STRING_BUFFER_CAPACITY;
  buf->len = 0;
  return buf;
}

/*
StringBuffer *StringBuffer_new(const char *str) {
  StringBuffer *buf = malloc(sizeof(StringBuffer));
  buf->len = strlen(str);
  buf->max_len = buf->len + DEFAULT_STRING_BUFFER_CAPACITY;
  buf->string = malloc(buf->max_len);
  strcpy(buf->string, str);
  return buf;
}

StringBuffer *StringBuffer_new(int capacity) {
  StringBuffer *buf = malloc(sizeof(StringBuffer));
  buf->string = malloc(capacity);
  buf->max_len = capacity;
  buf->len = 0;
  return buf;
}
*/

void StringBuffer_destroy(StringBuffer *self) {
  if (self->string) free(self->string);
  free(self);
}

void resize_if_needed(StringBuffer *self, int extra) {
  int newlen = self->len + extra + 1; // +1 for the null terminator
  if (newlen > self->max_len) {
    self->max_len = newlen + DEFAULT_STRING_BUFFER_CAPACITY;
    UChar *tmp = realloc(self->string, self->max_len * sizeof(UChar));
    if (!tmp) {
      puts("ERROR: Unable to allocate required memory");
      exit(1);
    } else {
      self->string = tmp;
    }
  }
}

StringBuffer *StringBuffer_append_string(StringBuffer *self, const UChar *str) {
  int32_t len = u_strlen(str);
  resize_if_needed(self, len);
  u_strcat(self->string, str);
  self->len += len;
  return self;
}

StringBuffer *StringBuffer_append_char(StringBuffer *self, UChar c) {
  resize_if_needed(self, 1);
  self->string[self->len++] = c;
  self->string[self->len] = 0;
  return self;
}

StringBuffer *StringBuffer_append_int(StringBuffer *self, int i) {
  char str[NUMBER_STR_LENGTH];
  UChar ustr[NUMBER_STR_LENGTH];
  sprintf(str, "%d", i);
  u_uastrcpy(ustr, str);
  return StringBuffer_append_string(self, ustr);
}

StringBuffer *StringBuffer_append_float(StringBuffer *self, float f) {
  char str[NUMBER_STR_LENGTH];
  UChar ustr[NUMBER_STR_LENGTH];
  sprintf(str, "%f", f);
  u_uastrcpy(ustr, str);
  return StringBuffer_append_string(self, ustr);
}

StringBuffer *StringBuffer_append_double(StringBuffer *self, double d) {
  char str[NUMBER_STR_LENGTH];
  UChar ustr[NUMBER_STR_LENGTH];
  sprintf(str, "%f", d);
  u_uastrcpy(ustr, str);
  return StringBuffer_append_string(self, ustr);
}

StringBuffer *StringBuffer_append_long(StringBuffer *self, long l) {
  char str[NUMBER_STR_LENGTH];
  UChar ustr[NUMBER_STR_LENGTH];
  sprintf(str, "%ld", l);
  u_uastrcpy(ustr, str);
  return StringBuffer_append_string(self, ustr);
}

int StringBuffer_length(StringBuffer *self) {
  return self->len;
}

UChar* StringBuffer_toString(StringBuffer *self) {
  UChar *rval = malloc((self->len + 1) * sizeof(UChar));
  u_strcpy(rval, self->string);
  return rval;
}
