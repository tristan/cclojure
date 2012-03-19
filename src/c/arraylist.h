#ifndef _arraylist_h
#define _arraylist_h

#include <unicode/ustdio.h>
#include "list.h"

#define _extend_ArrayList \
  _extend_List; \
  Object **_obj; \
  int _size; \
  int _arrsize

struct ArrayList {
  _extend_ArrayList;
};

typedef struct ArrayList ArrayList;

ArrayList *ArrayList_new(int initial_capacity);

#endif
