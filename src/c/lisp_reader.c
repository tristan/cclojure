#include <stdio.h>
#include <ctype.h>
#include "numbers.h"
#include "regex.h"
#include "string_buffer.h"

static regex_t *int_regex = NULL;
static regex_t *float_regex = NULL;
static regex_t *ratio_regex = NULL;

static void init() {
  if (int_regex == NULL) {
    int_regex = re_compile("([-+]?)(?:(0)|([1-9][0-9]*)|0[xX]([0-9A-Fa-f]+)|0([0-7]+)|([1-9][0-9]?)[rR]([0-9A-Za-z]+)|0[0-9]+)");
  }

  if (float_regex == NULL) {
    float_regex = re_compile("([-+]?[0-9]+(\\.[0-9]*)?([eE][-+]?[0-9]+)?)(M)?");
  }

  if (ratio_regex == NULL) {
    ratio_regex = re_compile("([-+]?[0-9]+)/([0-9]+)");
  }
}

static void destroy() {
  regex_destroy(int_regex);
  regex_destroy(float_regex);
  regex_destroy(ratio_regex);
}

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

static void *match_number(const char* s) {

  regmatch_t *match = re_match(int_regex, s);
  if (match) {
    char *n = NULL;
    n = re_group(match, 2);
    if (n != NULL) {
      return Integer_init_int(0);
      free(n);
      return NULL;
    }
    int negate = 1;
    n = re_group(match, 1);
    if (n != NULL) {
      if (n[0] == '-')
        negate = -1;
      free(n);
    }
    int radix = 10;
    if ((n = re_group(match, 3)) != NULL) {
      radix = 10;
    } else if ((n = re_group(match, 4)) != NULL) {
      radix = 16;
    } else if ((n = re_group(match, 5)) != NULL) {
      radix = 8;
    } else if ((n = re_group(match, 7)) != NULL) {
      char *tmp = re_group(match, 6);
      radix = atoi(tmp);
      free(tmp);
    }
    if (n == NULL) {
      return NULL;
    } else {
      number *num = Integer_init_str(n);
      free(n);
      return num;
    }
  }
  regmatch_destroy(match);

  match = re_match(float_regex, s);
  if (match) {
    char *n = NULL;
    if ((n = re_group(match, 4)) != NULL) {
      return Float_init_str(n);
      free(n);
    } else {
      return Float_init_str(s);
    }
  }
  regmatch_destroy(match);

  match = re_match(ratio_regex, s);
  if (match) {
    int inum, iden;
    char *snum, *sden;
    snum = re_group(match, 1);
    sden = re_group(match, 2);
    number *rat = Ratio_init_str(snum, sden);
    
    free(snum);
    free(sden);

    return rat;
  }
  return NULL;
}


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
  number *n = match_number(str);

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

  init();

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

