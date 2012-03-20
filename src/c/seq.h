#ifndef _seq_h
#define _seq_h

#include "object.h"

#define _extend_Seq \
  _extend_Object; \
  Seq *(*empty)(void *self); \
  int (*equiv)(const void *self, const void *obj); \
  int (*count)(const void *self); \
  Seq *(*seq)(const void *self); \
  Seq *(*cons)(void *self, Object *o); \
  Object *(*first)(const void *self); \
  Seq *(*next)(const void *self); \
  Seq *(*more)(const void *self)

// TODO: we may need the list stuff

typedef struct Seq Seq;

struct Seq {
  _extend_Seq;
};

//void Seq_destroy(void *self);
UChar *Seq_toString(const void *self);
int Seq_getClass(const void *self);
int Seq_instanceOf(const void *self, int class);
int Seq_equals(const void *self, const void *obj);
void *Seq_clone(const void *self);
Seq *Seq_empty(void *self);
int Seq_equiv(const void *self, const void *obj);
int Seq_count(const void *self);
Seq *Seq_seq(const void *self);
Seq *Seq_cons(void *self, Object *o);
Seq *Seq_more(const void *self);

//if (!T##Proto.) T##Proto. = Seq_;

#define _super_Seq_new(T, O) \
  if (!T##Proto.destroy) T##Proto.destroy = Object_destroy; \
  if (!T##Proto.toString) T##Proto.toString = Seq_toString; \
  if (!T##Proto.getClass) T##Proto.getClass = Seq_getClass; \
  if (!T##Proto.instanceOf) T##Proto.instanceOf = Seq_instanceOf; \
  if (!T##Proto.equals) T##Proto.equals = Seq_equals; \
  if (!T##Proto.clone) T##Proto.clone = Seq_clone; \
  if (!T##Proto.empty) T##Proto.empty = Seq_empty; \
  if (!T##Proto.equiv) T##Proto.equiv = Seq_equiv; \
  if (!T##Proto.count) T##Proto.count = Seq_count; \
  if (!T##Proto.seq) T##Proto.seq = Seq_seq; \
  if (!T##Proto.cons) T##Proto.cons = Seq_cons; \
  if (!T##Proto.more) T##Proto.more = Seq_more; \
  T *O = calloc(1, sizeof(T)); \
  *((Seq*)O) = T##Proto; \
  O->__refcount = 1

#endif
