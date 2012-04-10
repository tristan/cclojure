#include <stdio.h>

#include "dtypes.h"
#include "utils.h"
#include "utf8.h"
#include "timefuncs.h"
#include "ios.h"
#include "clojure.h"
#include "repl.h"
#include "lispreader.h"

void c_repl(ios_t *input) {
  char *namespace = "user";
  while (1) {
    printf("%s=> ", namespace);
    value_t o = lispreader_read(input, 1, IOS_EOF, 0);
    if (o == IOS_EOF) {
      break;
    }
  }
  puts("");
}
