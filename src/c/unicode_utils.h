#ifndef _unicode_utils_h
#define _unicode_utils_h

#include <unicode/ustring.h>
#include <unicode/ustdio.h>

int32_t u_utoi(UChar *u);

#define u_fseek(f, offset, whence) fseek(u_fgetfile(f), offset, whence)
#define u_printf(s, ...) u_fprintf(u_finit(stdout, NULL, NULL), s, __VA_ARGS__)

#endif
