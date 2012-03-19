#include <stdio.h>
#include "line_number_reader.h"

UChar LineNumberReader_read(void *self) {
  Reader *r = ((LineNumberReader*)self)->reader;
  UChar ch = r->read(r);
  if (ch == '\n') {
    ((LineNumberReader*)self)->_linenum++;
  }
  return ch;
}

UChar LineNumberReader_unread(void *self, UChar ch) {
  Reader *r = ((LineNumberReader*)self)->reader;
  UChar ch2 = r->unread(r, ch);
  if (ch2 == '\n') {
    ((LineNumberReader*)self)->_linenum--;
  }
  return ch2;
}

void LineNumberReader_reset(void *self) {
  Reader *r = ((LineNumberReader*)self)->reader;
  r->reset(r);
  ((LineNumberReader*)self)->_linenum = 0;
}

long LineNumberReader_skip(void *self, long n) {
  UChar ch;
  int i;
  for (i = 0; i < n; i++) {
    ch = LineNumberReader_read(self);
    if (ch == -1) {
      break;
    }
  }
  return i;
}

int LineNumberReader_instanceOf(void *self, int class) {
  return class == LINENUMBERREADER_CLASS || Reader_instanceOf(self, class);
}

int LineNumberReader_getClass(void *self) {
  return LINENUMBERREADER_CLASS;
}

UChar *LineNumberReader_toString(void *self) {
  UChar *str = malloc(sizeof(UChar) * 26); // address should be 8 chars
  u_sprintf(str, "LineNumberReader@%x", self);
  return str;
}

int LineNumberReader_getLineNumber(void *self) {
  return ((LineNumberReader*)self)->_linenum;
}

void LineNumberReader_destroy(void *self) {
  drop_ref(((LineNumberReader*)self)->reader);
  free(self);
}

Reader LineNumberReaderProto = {
  .getClass = LineNumberReader_getClass,
  .toString = LineNumberReader_toString,
  .instanceOf = LineNumberReader_instanceOf,
  .destroy = LineNumberReader_destroy,

  .read = LineNumberReader_read,
  .unread = LineNumberReader_unread,
  .reset = LineNumberReader_reset,
  .skip = LineNumberReader_skip
};

LineNumberReader *LineNumberReader_new(Reader *reader) {
  _super_Reader_new(LineNumberReader, obj);
  obj->getLineNumber = LineNumberReader_getLineNumber;

  obj->reader = (Reader*)reader;
  add_ref(reader);
  obj->_linenum = 0;
  return obj;
}
