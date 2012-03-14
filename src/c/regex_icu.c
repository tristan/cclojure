#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unicode/uregex.h>
#include "regex.h"

#define _r(N) (URegularExpression*)N->regex_data
#define _m(N) (URegularExpression*)N->match_data

/*
// builds a UChar* from a char*
static UChar *build_UChar(const char *str) {
  int len = strlen(str) + 1;
  UChar *rval = calloc(len, sizeof(UChar));
  for (int i = 0; i < len; i++) {
    rval[i] = str[i];
  }
  return rval;
}

// builds a char* from a UChar*
static char *build_char(const UChar *str) {
  int len = 0;
  while (str[len] != 0) len++; // strlen for UChar

  char *rval = calloc(len, sizeof(char));
  for (int i = 0; i < len; i++) {
    rval[i] = str[i];
  }
  return rval;
}
*/

regex_t *re_compile(const UChar *re_str) {
  UErrorCode status = 0;
  UParseError pe;

  URegularExpression *re = uregex_open(re_str,
					-1,
					0,
					&pe,
					&status);

  if (status != 0) {
    printf("ERROR compiling Regular Expression: %s\n", (char *)pe.preContext);
    return NULL;
  }

  regex_t *rval = malloc(sizeof(regex_t));
  rval->regex_data = (void*)re;
  return rval;
}

void regex_destroy(regex_t *reg) {
  if (_r(reg)) {
    uregex_close(_r(reg));
  }
  free(reg);
}

regmatch_t *re_match(regex_t *reg, const UChar* str) {
  if (str == NULL) {
    return NULL;
  }

  UErrorCode status = 0;

  uregex_setText(_r(reg), str, -1, &status);

  if (status != 0) {
    printf("ERROR setting string, error no: %d\n", status);
    return NULL;
  }

  if (uregex_matches(_r(reg), -1, &status)) {
    regmatch_t *m = calloc(1, sizeof(regmatch_t));
    m->match_data = _r(reg);
    return m;
  } else {
    return NULL;
  }
}

void regmatch_destroy(regmatch_t *match) {
  if (match != NULL) {
    free(match);
  }
}

int re_group_count(regmatch_t *match) {
  if (match) {
    UErrorCode status = 0;
    return uregex_groupCount(_m(match), &status);
  }
  return 0;
}

UChar *re_group(regmatch_t *match, int group) {
  // assuming the caller won't have any match data unless re_match was successful
  if (match) {
    UChar *resp;
    UErrorCode status;
    int s = uregex_start(_m(match), group, &status);
    int e = uregex_end(_m(match), group, &status);
    if (s >= 0) {
      int len = (e - s) + 1;
      resp = malloc(sizeof(UChar) * len);
      len = uregex_group(_m(match), group, resp, len, &status);
      if (len >= 0) {
        return resp;
      }
      puts("unexpectedly failed to get group");
    }
  }
  return NULL;
}
