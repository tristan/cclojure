#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <wctype.h>
#include <sys/types.h>
#include <locale.h>
#include <limits.h>
#include <errno.h>
#include <math.h>
#include "llt.h"
#include "clojure.h"
#include "opcodes.h"

struct test_t {
  union {
    void *v;
    char c[1];
  };
};

int main(int argc, char *argv[])
{
  printf("%ld %ld %ld\n", sizeof(void*), sizeof(char), sizeof(struct test_t));
  return 0;
}
