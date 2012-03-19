#include <string.h>
#include "arraylist.h"

int ArrayList_getClass(void *self) {
  return ARRAYLIST_CLASS;
}

int ArrayList_instanceOf(void *self, int class) {
  return class == ARRAYLIST_CLASS || List_instanceOf(self, class);
}

static int resize_internal(void *self, int size) {
  if (size < ((ArrayList*)self)->_arrsize) {
    return 1;
  }
  // double the size instead
  if (size < ((ArrayList*)self)->_arrsize << 1) {
    size = ((ArrayList*)self)->_arrsize << 1;
  }
  Object **tmp = realloc(((ArrayList*)self)->_obj, size * sizeof(Object*));
  if (!tmp) {
    return 0;
  }
  // set the newly allocated data to 0
  memset(((ArrayList*)self)->_obj + ((ArrayList*)self)->_arrsize, 
         0, (size - ((ArrayList*)self)->_arrsize) * sizeof(Object*));
  ((ArrayList*)self)->_obj = tmp;
  ((ArrayList*)self)->_arrsize = size;
  return 1;
}

int ArrayList_add_at(void *self, int index, Object *e) {
  if (index < 0 || index > ((ArrayList*)self)->_size) {
    printf("invalid index\n");
    return 0;
  }
  if (!resize_internal(self, index)) {
    return 0;
  }
  if (((ArrayList*)self)->_obj[index]) { // if something is already at index
    for (int i = ((ArrayList*)self)->_size - 1; i > index; --i) {
      ((ArrayList*)self)->_obj[i] = ((ArrayList*)self)->_obj[i-1]; 
    }
  }
  ((ArrayList*)self)->_obj[index] = e;
  ((ArrayList*)self)->_size += 1;
  return 1;
}

int ArrayList_add(void *self, Object *e) {
  return ArrayList_add_at(self, ((ArrayList*)self)->_size, e);
}

void ArrayList_clear(void *self) {
  int size = ((ArrayList*)self)->_size;
  for (int i = 0; i < size; i++) {
    Object *o = ((ArrayList*)self)->_obj[i];
    o->destroy(o);
    ((ArrayList*)self)->_obj[i] = 0;
  }
  ((ArrayList*)self)->_size = 0;
}

Object *ArrayList_get(void *self, int index) {
  if (index >= ((ArrayList*)self)->_size) {
    return NULL;
  }
  return ((ArrayList*)self)->_obj[index];
}

Object *ArrayList_remove(void *self, int index) {
  if (index >= ((ArrayList*)self)->_size) {
    return NULL;
  }
  Object *o = ((ArrayList*)self)->_obj[index];
  for (int i = index+1; i < ((ArrayList*)self)->_size; i++) {
    ((ArrayList*)self)->_obj[i-1] = ((ArrayList*)self)->_obj[i];
  }
  ((ArrayList*)self)->_obj[((ArrayList*)self)->_size] = 0;
  ((ArrayList*)self)->_size -= 1;
  return o;
}

Object *ArrayList_set(void *self, int index, Object *element) {
  if (index < 0 || index > ((ArrayList*)self)->_size) {
    printf("invalid index\n");
    return 0;
  }
  if (!resize_internal(self, index)) {
    return 0;
  }
  Object *r = ((ArrayList*)self)->_obj[index];
  ((ArrayList*)self)->_obj[index] = element;
  return r;
}

int ArrayList_size(void *self) {
  return ((ArrayList*)self)->_size;
}

List ArrayListProto = {
  .getClass = ArrayList_getClass,
  .instanceOf = ArrayList_instanceOf,

  .add = ArrayList_add,
  .add_at = ArrayList_add_at,
  .clear = ArrayList_clear,
  .get = ArrayList_get,
  .remove = ArrayList_remove,
  .set = ArrayList_set,
  .size = ArrayList_size,
};

ArrayList *ArrayList_new(int initial_capacity) {
  _super_List_new(ArrayList, obj);
  obj->_size = 0;
  obj->_arrsize = initial_capacity;
  obj->_obj = calloc(initial_capacity, sizeof(Object *));
  if (!obj->_obj) {
    obj->destroy(obj);
    return NULL;
  }
  return obj;
}
