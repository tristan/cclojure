#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "regex.h"
#include "numbers.h"

static regex_t *re = NULL;

void init() {
  const char *pat = 
    "([-+]?)(?:(0)|([1-9][0-9]*)|0[xX]([0-9A-Fa-f]+)|0([0-7]+)|([1-9][0-9]?)[rR]([0-9A-Za-z]+)|0[0-9]+)"; 

  if (re == NULL) {
    re = re_compile(pat);
  }
}

void destroy() {
  if (re != NULL)
    regex_destroy(re);
}

int regex_test(int argc, char *argv[]) {
  regmatch_t *matches = re_match(re, argv[1]);

  if (matches) { 
    int num = re_group_count(matches)+1;
    printf("got %d matches\n", num);
    for (int i = 0; i < num; i++) {
      char *g = re_group(matches, i);
      printf("match(%d): %s\n", i, g);
      free(g);
    }
  } else {
    puts("NO MATCHES");
  }

  regmatch_destroy(matches);

  return 0;
}



int numbers_test(int argc, char *argv[]) {

  if (argc < 5) {
    puts("need four inputs: an int, a float, and a dividend and divisor");
    return 1;
  }

  Number *i = Integer_init_str(argv[1], 10);
  Integer_negate(i);
  Number *f = Float_init_str(argv[2]);
  Number *r = Ratio_init_str(argv[3], argv[4]);

  char *is = Number_to_str(i);
  char *fs = Number_to_str(f);
  char *rs = Number_to_str(r);

  printf("Integer: %s\nFloat: %s\nRatio: %s\n", is, fs, rs);

  free(is);
  free(fs);
  free(rs);

  Number_destroy(i);
  Number_destroy(f);
  Number_destroy(r);

  return 0;
}

int main(int argc, char *argv[]) {
  int rval = 0;

  //init();

  //rval = regex_test(argc, argv);

  rval = numbers_test(argc, argv);

  //destroy();

  return rval;
}
