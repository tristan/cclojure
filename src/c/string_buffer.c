#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "string_buffer.h"

#define NUMBER_STR_LENGTH 32

StringBuffer *StringBuffer_new() {
  StringBuffer *buf = malloc(sizeof(StringBuffer));
  buf->string = malloc(DEFAULT_STRING_BUFFER_CAPACITY);
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
    char *tmp = realloc(self->string, self->max_len);
    if (!tmp) {
      puts("ERROR: Unable to allocate required memory");
      exit(1);
    } else {
      self->string = tmp;
    }
  }
}

StringBuffer *StringBuffer_append_string(StringBuffer *self, const char *str) {
  int len = strlen(str);
  resize_if_needed(self, len);
  strcat(self->string, str);
  self->len += len;
  return self;
}

StringBuffer *StringBuffer_append_char(StringBuffer *self, char c) {
  resize_if_needed(self, 1);
  self->string[self->len++] = c;
  self->string[self->len] = '\0';
  return self;
}

StringBuffer *StringBuffer_append_int(StringBuffer *self, int i) {
  char str[NUMBER_STR_LENGTH];
  sprintf(str, "%d", i);
  return StringBuffer_append_string(self, str);
}

StringBuffer *StringBuffer_append_float(StringBuffer *self, float f) {
  char str[NUMBER_STR_LENGTH];
  sprintf(str, "%f", f);
  return StringBuffer_append_string(self, str);
}

StringBuffer *StringBuffer_append_double(StringBuffer *self, double d) {
  char str[NUMBER_STR_LENGTH];
  sprintf(str, "%f", d);
  return StringBuffer_append_string(self, str);
}

StringBuffer *StringBuffer_append_long(StringBuffer *self, long l) {
  char str[NUMBER_STR_LENGTH];
  sprintf(str, "%ld", l);
  return StringBuffer_append_string(self, str);
}

int StringBuffer_length(StringBuffer *self) {
  return self->len;
}

char* StringBuffer_to_string(StringBuffer *self) {
  char *rval = malloc(self->len + 1);
  strcpy(rval, self->string);
  return rval;
}
