#include <setjmp.h>
#include <stdio.h>
#include <stdarg.h>
#include "llt.h"
#include "clojure.h"

void lerror(jmp_buf toplevel, char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  longjmp(toplevel, 1);
}
