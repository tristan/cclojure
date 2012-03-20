#include "cons.h"

Object *Cons_first(const void *self) {
  return ((Cons*)self)->_first;
}

Seq *Cons_next(const void *self) {
  Seq *m = ((Cons*)self)->more(self);
  return m->seq(m);
}

Seq *Cons_more(const void *self) {
  if (((Cons*)self)->_more == NULL) {
    // return empty list
  }
  return ((Cons*)self)->_more;
}


Seq ConsProto = {
  .first = Cons_first,
  .next = Cons_next,
  .more = Cons_more
  // TODO: count
};

Cons *Cons_new(Object *first, Seq *more) {
  _super_Seq_new(Cons, obj);
  obj->_first = first;
  obj->_more = more;
  return obj;
}
