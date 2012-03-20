#ifndef _unicode_utils_h
#define _unicode_utils_h

#include <unicode/ustring.h>
#include <unicode/ustdio.h>

int32_t u_utoi(UChar *u);

#define u_printf(s, ...) \
  UFILE* u____stdout = u_finit(stdout, NULL, NULL); \
  u_fprintf(u____stdout, s, __VA_ARGS__); \
  u_fclose(u____stdout)


#endif
