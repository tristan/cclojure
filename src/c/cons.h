#ifndef _cons_h
#define _cons_h

#include "seq.h"

#define _extend_Cons \
  _extend_Seq; \
  Object *_first; \
  Seq *_more

typedef struct Cons Cons;

struct Cons {
  _extend_Cons;
};

Cons *Cons_new(Object *first, Seq *more);

#endif
