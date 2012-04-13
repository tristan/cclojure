#include <setjmp.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#include "llt.h"
#include "clojure.h"

void lerror(jmp_buf toplevel, char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  fprintf(stderr, "\n");
  va_end(args);
  longjmp(toplevel, 1);
}


value_t *alloc_words(int n) {

  // TODO: alloc mem on the heap

  // can i simply use the stack as mock gc to start with?
  // the base types in clj are immutable, so as soon
  // as they're taken off the stack they should be free'd
  // by the function that removed them.

  // the heap is used to reduce the number of mallocs needed
  // technically i can just use malloc to start with
  // and add in a heap later on for optimization

  assert(n > 0);
  //n = LLT_ALIGN(n, 2); // TODO: is this necessary when using malloc only?
  value_t *first = (value_t*)malloc(n);
  return first;
}
