#include <stdio.h>
#include <ctype.h>
//#include <sys/types.h>
//#include <gmp.h>
#include "regex.h"
#include "string_buffer.h"

int initialized = 0;
static regex_t *int_regex;
static regex_t *float_regex;
static regex_t *ratio_regex;

static void init_patterns() {

  int number_of_patterns = 3;
  regex_t *regexs[] = {
    int_regex,
    float_regex,
    ratio_regex
  };

  char *patterns[] = {
    "([-+]?)(?:(0)|([1-9][0-9]*)|0[xX]([0-9A-Fa-f]+)|0([0-7]+)|([1-9][0-9]?)[rR]([0-9A-Za-z]+)|0[0-9]+)",
    "([-+]?[0-9]+(\\.[0-9]*)?([eE][-+]?[0-9]+)?)(M)?",
    "([-+]?[0-9]+)/([0-9]+)"
  };

  for (int i = 0; i < number_of_patterns; i++) {
    char *pattern = patterns[i];
    regex_t *reg = regexs[i];
    reg = re_compile(pattern);
  }

  initialized = 1;
  return;
}

/*
static char *regex_group(regmatch_t *match, const char* s) {
  int len = match->rm_eo - match->rm_so;
  char *tmp = malloc(len + 1);
  strncpy(tmp, s+match->rm_so, len);
  tmp[len] = '\0';
  return tmp;
}
*/

static int iswhitespace(int ch) {
  if (isblank(ch) || ch == ',') {
    return 1;
  } else {
    return 0;
  }
}

static int ismacro(int ch) {
  // TODO:
  return 0;
}

/*
static void *match_number(const char* s) {
  regmatch_t matches[8];
  if (regexec(&int_pattern, s, 8, matches, 0) != REG_NOMATCH) {
    long long int *i = malloc(sizeof(long long int));
    if (matches[2].rm_so == -1) {
      *i = 0;
      return i;
    }
    int negate = 1;
    if (matches[1].rm_so != -1) {
      if (s[matches[1].rm_so] == '-') {
        negate = -1;
      }
    }
    char *n = NULL;
    int radix = 10;
    if (matches[3].rm_so != -1) {
      n = regex_group(&matches[3], s);
      radix = 10;
    } else if (matches[4].rm_so != -1) {
      n = regex_group(&matches[4], s);
      radix = 16;
    } else if (matches[5].rm_so != -1) {
      n = regex_group(&matches[5], s);
      radix = 8;
    } else if (matches[7].rm_so != -1) {
      n = regex_group(&matches[7], s);
      char *tmp = regex_group(&matches[6], s);
      radix = atoi(tmp);
      free(tmp);
    }
    if (n == NULL) {
      return NULL;
    } else {
      *i = strtoll(n, NULL, radix) * negate;
      free(n);
      return i;
    }
  }

  if (regexec(&float_pattern, s, 5, matches, 0) != REG_NOMATCH) {
    long double *d = malloc(sizeof(long double));
    char *n = NULL;
    if (matches[4].rm_so != -1) {
      n = regex_group(&matches[1], s);
      *d = strtold(n, NULL);
      free(n);
    } else {
      *d = strtold(s, NULL);
    }
    return d;
  }

  if (regexec(&ratio_pattern, s, 3, matches, 0) != REG_NOMATCH) {
    int inum, iden;
    char *snum, *sden;
    snum = regex_group(&matches[1], s);
    sden = regex_group(&matches[2], s);
    inum = atoi(snum);
    iden = atoi(sden);
    free(snum);
    free(sden);

    if (inum % iden == 0) {
      int *i = malloc(sizeof(int));
      *i = inum / iden;
      return i;
    } else {
      double *d = malloc(sizeof(double));
      *d = inum / iden;
      return d;
    }
  }
  return NULL;
}
*/
static void *read_number(FILE* in, unsigned char ch) {

  StringBuffer *sb = StringBuffer_new();
  StringBuffer_append_char(sb, ch);

  for (;;) {
    int i = fgetc(in);
    ch = (unsigned char)i;
    if (i == EOF || iswhitespace(i) || ismacro(i)) {
      fseek(in, -1, SEEK_CUR);
      break;
    }
    StringBuffer_append_char(sb, ch);
  }

  char *str = StringBuffer_to_string(sb);
  void *n = NULL;//match_number(str);

  if (n == NULL) {
    printf("Invalid Number: %s", str);
  }

  free(str);
  StringBuffer_destroy(sb);
  return n;
}

void *parse_lisp(FILE *in) {
  int i;
  unsigned char ch;

  if (!initialized) {
    init_patterns();
  }

  for (;;) {
    while (iswhitespace(i = fgetc(in))) ;

    if (i == EOF) {
      puts("EOF");
      return NULL;
    }

    ch = (unsigned char)i;

    if (isdigit(ch)) {
      return read_number(in, ch);
    }
  }

  return NULL;
}

