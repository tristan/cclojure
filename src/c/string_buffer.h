#ifndef _my_string_h
#define _my_string_h

#include <string.h>
#include <stdlib.h>

#define DEFAULT_STRING_BUFFER_CAPACITY 32

struct StringBuffer {
  char *string;
  int len; // length of the string not including the null-terminator (i.e. the same as strlen would output)
  int max_len;
};

typedef struct StringBuffer StringBuffer;

StringBuffer *StringBuffer_new();
//StringBuffer *StringBuffer_new(const char *str);
//StringBuffer *StringBuffer_new(int capacity);

void StringBuffer_destroy(StringBuffer *self);

StringBuffer *StringBuffer_append_string(StringBuffer *self, const char *str);
StringBuffer *StringBuffer_append_char(StringBuffer *self, char c);
StringBuffer *StringBuffer_append_int(StringBuffer *self, int i);
StringBuffer *StringBuffer_append_float(StringBuffer *self, float f);
StringBuffer *StringBuffer_append_double(StringBuffer *self, double d);
StringBuffer *StringBuffer_append_long(StringBuffer *self, long l);


int StringBuffer_length(StringBuffer *self);

char* StringBuffer_to_string(StringBuffer *self);

#endif
