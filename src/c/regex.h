#ifndef _my_regex_h
#define _my_regex_h

#define STRTYPE UChar

struct regex_t {
  void *regex_data;
};

typedef struct regex_t regex_t;

regex_t *re_compile(STRTYPE *re_str);
void re_destroy(regex_t *reg);
char **re_match(regex_t *reg, STRTYPE* str, int *num);

#endif
