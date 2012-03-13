#ifndef _my_regex_h
#define _my_regex_h

struct regex_t {
  void *regex_data;
};

struct regmatch_t {
  void *match_data;
};

typedef struct regex_t regex_t;
typedef struct regmatch_t regmatch_t;

regex_t *re_compile(const char *re_str);
void regex_destroy(regex_t *reg);

regmatch_t *re_match(regex_t *reg, const char* str);
void regmatch_destroy(regmatch_t *match);

int re_group_count(regmatch_t *match);
char *re_group(regmatch_t *match, int group);

#endif
