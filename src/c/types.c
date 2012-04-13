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

value_t mk_stringn(char *str, size_t sz) {
  value_t v = mk_value(sz);
  memcpy(typedvalue_data(v), str, sz);
  return v;
}

value_t mk_string(char *str) {
  size_t sz = strlen(str) + 1;
  return mk_stringn(str, sz);
}

value_t mk_integer(char *i) {
  return 0;
}

value_t mk_decimal(char *d, char *exp) {
  return 0;
}

value_t mk_ratio(char *n, char *d) {
  return 0;
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
