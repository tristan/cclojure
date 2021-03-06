#include <stdio.h>
#include <setjmp.h>
#include "llt.h"
#include "clojure.h"
#include "repl.h"
#include "lispreader.h"

void c_repl(ios_t *input) {
  value_t ns = namespace("user");
  while (1) {
    ios_printf(ios_stdout, "%s=> ", namespace_name(ns));
    ios_flush(ios_stdout);
    value_t o = lispreader_read(input, ns, 0, IOS_EOF, 0);
    if (o == IOS_EOF) {
      break;
    }
    print_typedval(ios_stdout, o);
  }
  puts("");
}
