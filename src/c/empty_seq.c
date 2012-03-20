#include "cons.h"
#include "empty_seq.h"

Object *EmptySeq_first(const void *self) {
  return NULL;
}

Seq *EmptySeq_next(const void *self) {
  return NULL;
}

Seq *EmptySeq_more(const void *self) {
  return (Seq*)self;
}

int EmptySeq_count(const void *self) {
  return 0;
}

Seq *EmptySeq_cons(void *self, Object *o) {
  return (Seq*)Cons_new(o, NULL);
}

Seq *EmptySeq_empty(void *self) {
  return (Seq*)self;
}

Seq *EmptySeq_seq(const void *self) {
  return NULL;
}

Seq EmptySeqProto = {
  .first = EmptySeq_first,
  .next = EmptySeq_next,
  .more = EmptySeq_more,
  .cons = EmptySeq_cons,
  .count = EmptySeq_count,
  .empty = EmptySeq_empty,
  .seq = EmptySeq_seq
};

EmptySeq *EmptySeq_new() {
  _super_Seq_new(EmptySeq, obj);
  return obj;
}
