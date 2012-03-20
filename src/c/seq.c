#include "cons.h"
#include "seq.h"

UChar *Seq_toString(const void *self) {
  return NULL;
}

int Seq_getClass(const void *self) {
  return SEQ_CLASS;
}

int Seq_instanceOf(const void *self, int class) {
  return class == SEQ_CLASS || Object_instanceOf(self, class);
}

int Seq_equals(const void *self, const void *obj) {
  if (self == obj) {
    return 1;
  }
  if (!((Object*)obj)->instanceOf(obj, SEQ_CLASS)) {
    return 0;
  }
  Seq *ms = ((Seq*)obj)->seq(obj); // TODO: seperate case for lazy-seq
  // TODO: make sure this is ok, reference counting wise
  for (Seq *s = ((Seq*)self)->seq(self); s != NULL; s = s->next(s), ms = ms->next(ms)) {
    if (ms == NULL) {
      return 0;
    }
    Object *sf = s->first(s);
    Object *msf = ms->first(ms);
    if (sf == msf) {
      return 1;
    }
    if (sf->equals(sf, msf)) {
      return 1;
    }
  }
  return ms == NULL;
}

void *Seq_clone(const void *self) {
  return NULL;
}

Seq *Seq_empty(void *self) {
  return NULL;
}

int Seq_equiv(const void *self, const void *obj) {
  return 0;
}

int Seq_count(const void *self) {
  int i = 1;
  for (Seq *s = ((Seq*)self)->next(self); s != NULL; s = s->next(s), i++) {
    // TODO: counted?
  }
  return i;
}

Seq *Seq_seq(const void *self) {
  return (Seq*)self;
}

Seq *Seq_cons(void *self, Object *o) {
  return (Seq*)Cons_new(o, self);
}

Seq *Seq_more(const void *self) {
  return NULL;
}
