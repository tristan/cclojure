#ifndef _empty_seq_h
#define _empty_seq_h

#include "seq.h"

#define _extend_EmptySeq \
  _extend_Seq

typedef struct EmptySeq EmptySeq;

struct EmptySeq {
  _extend_EmptySeq;
};

EmptySeq *EmptySeq_new();

#endif
