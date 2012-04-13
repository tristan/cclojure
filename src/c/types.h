#ifndef _TYPES_H
#define _TYPES_H

typedef uptrint_t value_t;

typedef struct {
  value_t first;
  value_t rest;
} list_t;

#define TAG_TYPEDVALUE 0x0
#define TAG_SYM 0x1
#define TAG_NS 0x2

#define tag(x) ((x)&0x7)
#define ptr(x) ((void*)((x)&(~(value_t)0x7)))
#define tagptr(p,t) (((value_t)(p)) | (t))

typedef enum { T_INTEGER, T_DECIMAL, T_RATIO, 
               T_STRING,
               T_LIST, T_VECTOR, T_MAP, T_SET,
               N_NUMTYPES } type_t;

typedef struct {
  type_t type;
  void *data;
  size_t len;
} typedvalue_t;

#define typedvalue_data(v) (((typedvalue_t*)ptr(v))->data)

value_t mk_stringn(char *str, size_t sz);
value_t mk_string(char *str);
value_t mk_integer(char *i);
value_t mk_decimal(char *d, char *exp);
value_t mk_ratio(char *n, char *d);

typedef struct _symbol_t {
  // TODO: not sure yet how to match this to the clojure symbol
  struct _symbol_t *left;
  struct _symbol_t *right;
  value_t namespace;
  union {
    char name[1];
    void *_pad; 
  };
} symbol_t;

typedef struct _namespace_t {
  struct _namespace_t *left;
  struct _namespace_t *right;
  symbol_t *name; // namespace name (TODO: maybe it's ok if this is a char*)
  symbol_t *mappings; // 
} namespace_t;

#define namespace_mappings(ns) (((namespace_t*)ptr(ns))->mappings)
#define namespace_name(ns) (((namespace_t*)ptr(ns))->name->name)

value_t symbol(value_t namespace, char *symstr);

value_t namespace(char *ns);

#endif
