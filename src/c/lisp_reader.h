#ifndef _lisp_reader_h
#define _lisp_reader_h

#include <unicode/ustdio.h>
#include "object.h"
#include "reader.h"

Object *parse_lisp(Reader *r, int eof_is_error, Object *eof_value, int is_recursive);
void LispReader_shutdown();

#endif
