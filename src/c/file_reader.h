#ifndef _file_reader_h
#define _file_reader_h

#include <unicode/ustdio.h>
#include "reader.h"

#define _extend_FileReader \
  _extend_Reader \
  UFILE *file;

struct FileReader {
  _extend_FileReader
};

typedef struct FileReader FileReader;

FileReader *FileReader_new();
int FileReader_init(void *self, void *ufile);
//UChar *Reader_toString(void *reader);
//int FileReader_getClass(void *self);
void FileReader_destroy(void *self);

UChar FileReader_read(void *self);
UChar FileReader_unread(void *self, UChar ch);
void FileReader_reset(void *self);
long FileReader_skip(void *self, long n);

#endif
