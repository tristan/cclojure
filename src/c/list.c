#include "string_buffer.h"
#include "list.h"

UChar *List_toString(void *self) {
  StringBuffer *sb = StringBuffer_new();
  const UChar header[] = { '#', '<', 'L', 'i', 's', 't', ' ', '[', 0 };
  const UChar sep[] = { ',', ' ', 0 };
  const UChar tail[] = { ']', '>', 0 };
  StringBuffer_append_string(sb, header);
  int size = ((List*)self)->size(self);
  for (int i = 0; i < size; i++) {
    Object *o = ((List*)self)->get(self, i);
    UChar *s = o->toString(o);
    int isstr = o->instanceOf(o, STRING_CLASS);
    if (isstr) StringBuffer_append_char(sb, '"');
    StringBuffer_append_string(sb, s);
    if (isstr) StringBuffer_append_char(sb, '"');
    if (i < size-1) {
      StringBuffer_append_string(sb, sep);
    }
    free(s);
  }
  StringBuffer_append_string(sb, tail);
  UChar *str = StringBuffer_toString(sb);
  StringBuffer_destroy(sb);
  return str;
}

int List_getClass(void *self) {
  return LIST_CLASS;
}

int List_instanceOf(void *self, int class) {
  return class == LIST_CLASS || Object_instanceOf(self, class);
}

int List_equals(void *self, void *list) {
  if (!((Object*)list)->instanceOf(list, LIST_CLASS)) {
    return 0;
  }
  int size = ((List*)self)->size(self);
  if (((List*)list)->size(list) != size) {
    return 0;
  }
  for (int i = 0; i < size; i++) {
    Object *o1 = ((List*)self)->get(self, i);
    Object *o2 = ((List*)list)->get(list, i);
    if (!o1->equals(o1, o2)) {
      return 0;
    }
  }
  return 1;
}

int List_indexOf(void *self, Object *o) {
  int size = ((List*)self)->size(self);
  for (int i = 0; i < size; i++) {
    if (o->equals(o, ((List*)self)->get(self, i))) {
      return i;
    }
  }
  return -1;
}

int List_remove_object(void *self, Object *o) {
  int idx = ((List*)self)->indexOf(self, o);
  if (idx < 0) {
    return 0;
  }
  Object *obj = ((List*)self)->remove(self, idx);
  drop_ref(obj);
  return 1;
}

int List_contains(void *self, Object *o) {
  return ((List*)self)->indexOf(self, o) >= 0;
}

int List_isEmpty(void *self) {
  return ((List*)self)->size(self) == 0;
}

Object **List_toArray(void *self) {
  int size = ((List*)self)->size(self);
  Object **rval = calloc(size, sizeof(Object*));
  for (int i = 0; i < size; i++) {
    Object *o = ((List*)self)->get(self, i);
    rval[i] = o;
  }
  return rval;
}

void List_destroy(void *self) {
  int size = ((List*)self)->size(self);
  for (int i = 0; i < size; i++) {
    Object *o = ((List*)self)->get(self, i);
    drop_ref(o);
  }
  free(self);
}
