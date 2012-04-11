#include <stdio.h>
#include <setjmp.h>
#include "llt.h"
#include "clojure.h"
#include "repl.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    ios_init_stdstreams();
    c_repl(ios_stdin);
  }
}
