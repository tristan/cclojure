#ifndef _LISP_READER_H
#define _LISP_READER_H

value_t lispreader_read(ios_t *in, char eof_is_error, value_t eof_value, char is_recursive);

#endif
