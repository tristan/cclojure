#include <stdio.h>
#include "file_reader.h"

UChar FileReader_read(void *self) {
  return u_fgetc(((FileReader*)self)->_file);
}

UChar FileReader_unread(void *self, UChar ch) {
  return u_fungetc(ch, ((FileReader*)self)->_file);
}

void FileReader_reset(void *self) {
  u_frewind(((FileReader*)self)->_file);
}

long FileReader_skip(void *self, long n) {
  FILE *f = u_fgetfile(((FileReader*)self)->_file);
  long s = ftell(f);
  if (fseek(f, n, SEEK_CUR) == 0) {
    return n;
  } else {
    long e = ftell(f);
    if (e >= s) {
      return e-s;
    } else {
      return -1;
    }
  }
}

int FileReader_instanceOf(const void *self, int class) {
  return class == FILEREADER_CLASS || Reader_instanceOf(self, class);
}

int FileReader_getClass(const void *self) {
  return FILEREADER_CLASS;
}

UChar *FileReader_toString(const void *self) {
  UChar *str = malloc(sizeof(UChar) * 20); // address should be 8 chars
  u_sprintf(str, "FileReader@%x", self);
  return str;
}

void FileReader_destroy(void *self) {
  if (((FileReader*)self)->_close) {
    u_fclose(((FileReader*)self)->_file);
  }
  free(self);
}

Reader FileReaderProto = {
  .destroy = FileReader_destroy,
  .getClass = FileReader_getClass,
  .toString = FileReader_toString,
  .instanceOf = FileReader_instanceOf,

  .read = FileReader_read,
  .unread = FileReader_unread,
  .reset = FileReader_reset,
  .skip = FileReader_skip
};


FileReader *FileReader_new_u(UFILE *f) {
  _super_Reader_new(FileReader, obj);
  obj->_file = f;
  obj->_close = 0;
  return obj;
}

FileReader *FileReader_new_f(FILE *f) {
  _super_Reader_new(FileReader, obj);
  obj->_file = u_finit(f, NULL, NULL);
  obj->_close = 0;
  return obj;
}

FileReader *FileReader_new_s(const char *f) {
  _super_Reader_new(FileReader, obj);
  obj->_file = u_fopen(f, "r", NULL, NULL);
  obj->_close = 1;
  return obj;
}
