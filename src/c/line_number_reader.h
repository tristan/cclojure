#ifndef _line_number_reader_h
#define _line_number_reader_h

#include <unicode/ustdio.h>
#include "reader.h"

#define _extend_LineNumberReader \
  _extend_Reader \
  int (*getLineNumber)(void *self); \
  Reader *reader; \
  int _linenum;

struct LineNumberReader {
  _extend_LineNumberReader
};

typedef struct LineNumberReader LineNumberReader;

LineNumberReader *LineNumberReader_new();
int LineNumberReader_init(void *self, void *reader);
//UChar *Reader_toString(void *reader);
//int LineNumberReader_getClass(void *self);
void LineNumberReader_destroy(void *self);

UChar LineNumberReader_read(void *self);
UChar LineNumberReader_unread(void *self, UChar ch);
void LineNumberReader_reset(void *self);
long LineNumberReader_skip(void *self, long n);

#endif
