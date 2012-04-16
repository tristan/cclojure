#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "llt.h"
#include "clojure.h"

static value_t mk_value(size_t sz) {
  typedvalue_t *ptv;
  if (sz == 0) {
    return 0; // don't even try to allocate if the size is 0
  }
  ptv = (typedvalue_t*)alloc_words(sizeof(typedvalue_t));
  assert(((value_t)ptv & 0x7) == 0); // make sure we're aligned correctly
  ptv->data = malloc(sz);
  ptv->len = sz;
  return tagptr(ptv, TAG_TYPEDVALUE);
}

value_t mk_list() {
  value_t v = mk_value(sizeof(list_t));
  lsize(v) = 0;
  lcar_(v) = 0;
  lcdr_(v) = 0;
  typedvalue_type(v) = T_LIST;
  return v;
}

value_t mk_stringn(char *str, size_t sz) {
  value_t v = mk_value(sz);
  typedvalue_type(v) = T_STRING;
  memcpy(typedvalue_data(v), str, sz);
  return v;
}

value_t mk_string(char *str) {
  size_t sz = strlen(str) + 1;
  return mk_stringn(str, sz);
}

value_t mk_integer(char *i, int base) {
  value_t v = mk_value(sizeof(int64_t));
  typedvalue_type(v) = T_INTEGER;
  *(int64_t*)typedvalue_data(v) = strtoll(i, &i+strlen(i), base);
  return v;
}

value_t mk_decimal(char *d) {
  value_t v = mk_value(sizeof(double));
  typedvalue_type(v) = T_DECIMAL;
  *(double*)typedvalue_data(v) = strtod(d, &d+strlen(d));
  return v;
}

typedef struct {
  int64_t num;
  int64_t den;
} ratio_t;

#define str_val(v) ((char*)typedvalue_data(v))
#define int_val(v) (*(int64_t*)typedvalue_data(v))
#define dec_val(v) (*(double*)typedvalue_data(v))
#define ratio_num(v) (((ratio_t*)typedvalue_data(v))->num)
#define ratio_den(v) (((ratio_t*)typedvalue_data(v))->den)

value_t mk_ratio(char *n, char *d) {
  value_t v = mk_value(sizeof(ratio_t));
  size_t ln = strlen(n);
  size_t ld = strlen(d);
  typedvalue_type(v) = T_RATIO;
  ratio_num(v) = strtoll(n, &n+ln, 10);
  ratio_den(v) = strtoll(d, &d+ld, 10);
  return v;
}

value_t reduce(value_t n) {
  //if (isratio(n)) {
    // TODO:
  //}
  return n;
}

void print_typedval(ios_t *out, value_t v) {
  if (isratio(v)) {
    ios_printf(out, "ratio: %ld/%lu\n", ratio_num(v), ratio_den(v));
  } else if (isinteger(v)) {
    ios_printf(out, "integer: %ld\n", int_val(v));
  } else if (isdecimal(v)) {
    ios_printf(out, "decimal: %f\n", dec_val(v));
  } else if (isstring(v)) {
    ios_printf(out, "string: %s\n", str_val(v));
  } else {
    ios_printf(out, "unknown value type at: %x\n", v);
  }
}


static symbol_t *mk_symbol(char *str) {
  size_t len = strlen(str);
  symbol_t *sym = (symbol_t*)malloc(sizeof(symbol_t)-sizeof(void*) + len + 1);
  assert(((uptrint_t)sym & 0x7) == 0); // make sure malloc aligns 8
  sym->left = sym->right = NULL;
  sym->namespace = 0;
  strcpy(&sym->name[0], str);
  return sym;
}

static symbol_t **symtab_lookup(symbol_t **ptree, char *str) {
  int x;
  while (*ptree != NULL) {
    x = strcmp(str, (*ptree)->name);
    if (x == 0)
      return ptree;
    if (x < 0)
      ptree = &(*ptree)->left;
    else
      ptree = &(*ptree)->right;
  }
  return ptree;
}

value_t symbol(value_t ns, char *str) {
  symbol_t **pnode;
  if (ns != 0) {
    return 0; // does it really make sence to make symbols without a default ns?
  }
  
  pnode = symtab_lookup(&namespace_mappings(ns), str);
  if (*pnode == NULL) {
    *pnode = mk_symbol(str);
    (*pnode)->namespace = ns;
  }
  return tagptr(*pnode, TAG_SYM);
}

static namespace_t *nstable = NULL;

static namespace_t *mk_namespace(char *str) {
  namespace_t *ns = (namespace_t*)malloc(sizeof(namespace_t));
  assert(((uptrint_t)ns & 0x7) == 0); // make sure malloc aligns 8
  ns->left = ns->right = NULL;
  ns->mappings = NULL;
  ns->name = mk_symbol(str);
  return ns;
}

// TODO: maybe we can merge symbol_t and namespace_t ?
static namespace_t **nstab_lookup(namespace_t **ptree, char *str) {
  int x;
  while (*ptree != NULL) {
    x = strcmp(str, (*ptree)->name->name);
    if (x == 0)
      return ptree;
    if (x < 0)
      ptree = &(*ptree)->left;
    else
      ptree = &(*ptree)->right;
  }
  return ptree;
}

value_t namespace(char *str) {
  namespace_t **pnode;
  pnode = nstab_lookup(&nstable, str);
  if (*pnode == NULL) {
    *pnode = mk_namespace(str);
  }
  return tagptr(*pnode, TAG_NS);
}
