#include <stdio.h>
#include <assert.h>
#include "lisp_reader.h"

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("USAGE: %s <input-file>\n", argv[0]);
    return 1;
  }

  UFILE *f = u_fopen(argv[1], "r", NULL, NULL);
  parse_lisp(f);

  return 0;
}
