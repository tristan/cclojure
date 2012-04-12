#include <string.h>
#include "llt.h"
#include "clojure.h"

static value_t mk_value(size_t sz) {
  typedvalue_t *ptv;
  if (sz == 0) {
    return 0; // don't even try to allocate if the size is 0
  }

  // TODO: alloc mem on the heap

  // can i simply use the stack as mock gc to start with?
  // the base types in clj are immutable, so as soon
  // as they're taken off the stack they should be free'd
  // by the function that removed them.

  // the heap is used to reduce the number of mallocs needed
  // technically i can just use malloc to start with
  // and add in a heap later on for optimization

  return 0;
}

value_t mk_string(char *str) {
  return mk_value(strlen(str));
}
value_t mk_integer(char *i) {
  return 0;
}
value_t mk_decimal(char *d, char *exp) {
  return 0;
}
value_t mk_ratio(char *n, char *d) {
  return 0;
}
