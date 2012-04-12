#include <stdio.h>
#include <setjmp.h>
#include "llt.h"
#include "clojure.h"
#include "repl.h"
#include "lispreader.h"

void c_repl(ios_t *input) {
  char *namespace = "user";
  while (1) {
    ios_printf(ios_stdout, "%s=> ", namespace);
    ios_flush(ios_stdout);
    value_t o = lispreader_read(input, 1, IOS_EOF, 0);
    if (o == IOS_EOF) {
      break;
    }
  }
  puts("");
}
