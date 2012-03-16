#ifndef _file_reader_h
#define _file_reader_h

#include <unicode/ustdio.h>
#include "reader.h"

#define _extend_FileReader \
  _extend_Reader; \
  UFILE *_file; \
  int _close

struct FileReader {
  _extend_FileReader;
};

typedef struct FileReader FileReader;

FileReader *FileReader_new_u(UFILE *f);
FileReader *FileReader_new_f(FILE *f);
FileReader *FileReader_new_s(const char *f);

#endif
