#ifndef _lisp_reader_h
#define _lisp_reader_h

#include <unicode/ustdio.h>
#include "object.h"

Object *parse_lisp(UFILE *fd, int eof_is_error, Object *eof_value, int is_recursive);

#endif
