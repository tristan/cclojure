#include <stdlib.h>
#include <stdio.h>
#include <unicode/uregex.h>
#include "regex.h"

int regex_test(int argc, char *argv[]) {
  puts("Hello");

  //UChar *patt = "([-+]?)(?:(0)|([1-9][0-9]*)|0[xX]([0-9A-Fa-f]+)|0([0-7]+)|([1-9][0-9]?)[rR]([0-9A-Za-z]+)|0[0-9]+)"; 
  //UChar *patt = "abc+";
  UChar patt[7] = {'a', 'b', '(', 'c', ')', '+', 0};
  //UChar *search = "abccc";
  UChar search[6] = {'a','b','c','c','c',0};

  regex_t *re = 
    re_compile(patt);

  int num = 0;
  char **matches = re_match(re, search, &num);

  if (num > 0) {
    for (int i = 0; i < num; i++) {
      printf("match(%d): %s", i, matches[i]);
    }
  } else {
    puts("NO MATCHES");
  }

  re_destroy(re);

  return 0;
}


int main(int argc, char *argv[]) {
  return regex_test(argc, argv);
}
