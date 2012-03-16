#ifndef _line_number_reader_h
#define _line_number_reader_h

#include <unicode/ustdio.h>
#include "reader.h"

#define _extend_LineNumberReader \
  _extend_Reader; \
  int (*getLineNumber)(void *self); \
  Reader *reader; \
  int _linenum

struct LineNumberReader {
  _extend_LineNumberReader;
};

typedef struct LineNumberReader LineNumberReader;

LineNumberReader *LineNumberReader_new(Reader *reader);

#endif
