#ifndef _TYPES_H
#define _TYPES_H

typedef uptrint_t value_t;

typedef struct {
  value_t first;
  value_t rest;
} list_t;

#define TAG_TYPEDVALUE 0x0

typedef enum { T_INTEGER, T_DECIMAL, T_RATIO, 
               T_STRING,
               T_LIST, T_VECTOR, T_MAP, T_SET,
               N_NUMTYPES } type_t;

typedef struct {
  type_t type;
  void *data;
  size_t len;
} typedvalue_t;

value_t mk_string(char *str);
value_t mk_integer(char *i);
value_t mk_decimal(char *d, char *exp);
value_t mk_ratio(char *n, char *d);

#endif
