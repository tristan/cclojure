#ifndef _CLOJURE_H
#define _CLOJURE_H

#include <setjmp.h>
#include "llt.h"
#include "types.h"

void lerror(jmp_buf toplevel, char *format, ...)  __attribute__ ((__noreturn__));

value_t *alloc_words(int n);

#endif
