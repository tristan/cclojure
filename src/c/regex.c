#include <stdlib.h>
#include <stdio.h>
#include <unicode/uregex.h>
#include "regex.h"

#define _(N) (URegularExpression*)N->regex_data

regex_t *re_compile(STRTYPE *re_str) {
  UErrorCode status = 0;
  UParseError pe;

  //const UChar *ure_str = (const UChar*)re_str;
  URegularExpression *ure = uregex_open(re_str,
					-1,
					0,
					&pe,
					&status);

  if (status != 0) {
    printf("ERROR compiling Regular Expression: %s\n", (char *)pe.preContext);
    return NULL;
  }

  regex_t *rval = malloc(sizeof(regex_t));
  rval->regex_data = (void*)ure;
  return rval;
}

void re_destroy(regex_t *reg) {
  if (reg->regex_data) {
    uregex_close(_(reg));
  }
  free(reg);
}

char **re_match(regex_t *reg, STRTYPE* str, int *matches) {
  if (str == NULL) {
    return NULL;
  }

  UErrorCode status = 0;

  uregex_setText(_(reg), str, -1, &status);

  if (status != 0) {
    printf("ERROR setting string, error no: %d\n", status);
    return NULL;
  }

  //if (uregex_matches(_(reg), -1, &status)) 
  if (uregex_find(_(reg), -1, &status)) {
    *matches = uregex_groupCount(_(reg), &status);
    printf("GOT: %d matches\n", *matches);
  }

  char *rval[] = { "" };

  return NULL;
}
