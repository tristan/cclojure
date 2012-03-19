#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "object.h"
#include "file_reader.h"
#include "line_number_reader.h"
#include "unicode_utils.h"
#include "lisp_reader.h"
#include "arraylist.h"

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("USAGE: %s <input-file>\n", argv[0]);
    return 1;
  }

  UFILE *f = u_fopen(argv[1], "r", NULL, NULL);
  FileReader *fr = FileReader_new_u(f);
  LineNumberReader *lnr = LineNumberReader_new((Reader*)fr);

  Object *o = parse_lisp((Reader*)lnr, 1, NULL, 1);

  ArrayList *list = ArrayList_new(8);
  list->add(list, (Object*)fr);
  list->add(list, (Object*)lnr);
  list->add(list, (Object*)o);

  UChar *s = list->toString(list);
  u_printf("%S\n", s);
  free(s);
  // list->destroy(list)

  o->destroy(o);

  lnr->destroy(lnr);
  fr->destroy(fr);
  u_fclose(f);

  return 0;
}
