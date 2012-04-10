#include <stdio.h>
#include "dtypes.h"
#include "utils.h"
#include "utf8.h"
#include "timefuncs.h"
#include "ios.h"
#include "clojure.h"
#include "repl.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    ios_init_stdstreams();
    c_repl(ios_stdin);
  }
}
