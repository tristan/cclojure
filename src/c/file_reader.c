#include <stdio.h>
#include "file_reader.h"

int FileReader_init(void *self, void *ufile) {
  ((FileReader*)self)->file = (UFILE*)ufile;
  return 1;
}

UChar FileReader_read(void *self) {
  return u_fgetc(((FileReader*)self)->file);
}

UChar FileReader_unread(void *self, UChar ch) {
  return u_fungetc(ch, ((FileReader*)self)->file);
}

void FileReader_reset(void *self) {
  u_frewind(((FileReader*)self)->file);
}

long FileReader_skip(void *self, long n) {
  FILE *f = u_fgetfile(((FileReader*)self)->file);
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

int FileReader_instanceOf(void *self, int class) {
  return class == FILEREADER_CLASS || Reader_instanceOf(self, class);
}

int FileReader_getClass(void *self) {
  return FILEREADER_CLASS;
}

UChar *FileReader_toString(void *self) {
  UChar *str = malloc(sizeof(UChar) * 20); // address should be 8 chars
  u_sprintf(str, "FileReader@%x", self);
  return str;
}

Reader FileReaderProto = {
  .init = FileReader_init,
  // .destroy = FileReader_destroy,
  .getClass = FileReader_getClass,
  .toString = FileReader_toString,
  .instanceOf = FileReader_instanceOf,

  .read = FileReader_read,
  .unread = FileReader_unread,
  .reset = FileReader_reset,
  .skip = FileReader_skip
};


FileReader *FileReader_new() {
  _super_Reader_new(FileReader, obj);
  
  return obj;
}
