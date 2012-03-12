#include <oniguruma.h>
#include "regex.h"

regex_t *re_compile(const char *re_str) {
  int r = 0;
  OnigErrorInfo einfo;

  const unsigned char *pattern = re_str;
  regex_t *reg = regexs[i];
  r = 
    onig_new(&reg, pattern, pattern + strlen((char* )pattern),
	     ONIG_OPTION_DEFAULT | ONIG_OPTION_EXTEND | ONIG_OPTION_FIND_LONGEST, 
	     ONIG_ENCODING_ASCII, ONIG_SYNTAX_JAVA, &einfo);
  if (r != ONIG_NORMAL) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str(s, r, &einfo);
    fprintf(stderr, "ERROR: %s\n", s);
    exit(1);
  }

  regex_t *rval = malloc(sizeof(regex_t));
  rval->regex_data = (void*)ure;
  return rval;
}

void re_destroy(regex_t *reg) {

}
