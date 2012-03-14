#ifndef _my_string_h
#define _my_string_h

#include <unicode/ustring.h>

struct StringBuffer {
  UChar *string;
  int32_t len; // length of the string not including the null-terminator (i.e. the same as strlen would output)
  int32_t max_len;
};

typedef struct StringBuffer StringBuffer;

StringBuffer *StringBuffer_new();
//StringBuffer *StringBuffer_new(const char *str);
//StringBuffer *StringBuffer_new(int capacity);

void StringBuffer_destroy(StringBuffer *self);

StringBuffer *StringBuffer_append_string(StringBuffer *self, const UChar *str);
StringBuffer *StringBuffer_append_char(StringBuffer *self, UChar c);
StringBuffer *StringBuffer_append_int(StringBuffer *self, int32_t i);
StringBuffer *StringBuffer_append_float(StringBuffer *self, float f);
StringBuffer *StringBuffer_append_double(StringBuffer *self, double d);
StringBuffer *StringBuffer_append_long(StringBuffer *self, long l);


int32_t StringBuffer_length(StringBuffer *self);

UChar* StringBuffer_to_string(StringBuffer *self);

#endif
