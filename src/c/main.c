#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "object.h"
#include "file_reader.h"
#include "line_number_reader.h"
#include "unicode_utils.h"
//#include "lisp_reader.h"

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("USAGE: %s <input-file>\n", argv[0]);
    return 1;
  }

  UFILE *f = u_fopen(argv[1], "r", NULL, NULL);
  FileReader *fr = FileReader_new();
  fr->init(fr, f);
  LineNumberReader *lnr = LineNumberReader_new();
  lnr->init(lnr, fr);

  //  Object *res = NULL;

  UChar *str = lnr->toString(lnr);
  u_printf("%S\n", str);
  free(str);

  lnr->destroy(lnr);
  fr->destroy(fr);
  u_fclose(f);

  return 0;
}
