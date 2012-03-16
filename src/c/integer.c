#include <string.h>
#include <unicode/ustring.h>
#include <gmp.h>
#include "numbers.h"

#define _i(N) *(mpz_t*)((Integer*)N)->_num
#define _d(N) *(mpf_t*)((Decimal*)N)->_num
#define _r(N) *(mpq_t*)((Ratio*)N)->_num

int Integer_init(void *self, void *num) {
  mpz_t* it = malloc(sizeof(mpz_t));
  mpz_init(*it);
  mpz_set_si(*it, *(int*)num);

  ((Integer*)self)->_num = it;
  return 1;
}

void Integer_destroy(void *self) {
  if (((Integer*)self)->_num != NULL) {
    mpz_clear(((Integer*)self)->_num);
  }
  free(self);
}

int Integer_instanceOf(void *self, int class) {
  return class == INTEGER_CLASS || Number_instanceOf(self, class);
}

int Integer_getClass(void *self) {
  return INTEGER_CLASS;
}

UChar *Integer_toString(void *self) {
  char *astr = mpz_get_str(NULL, 10, _i(self));
  if (astr) {
    UChar *ustr = malloc(sizeof(UChar) * strlen(astr));
    u_uastrcpy(ustr, astr);
    free(astr);
    return ustr;
  }
  return NULL;
}

int Integer_equals(void *self, void* obj) {
  if (((Object*)self)->getClass(self) != Integer_getClass(self)) {
    puts("calling Integer_equals on non integer object is strange!");
  } else {
    switch (((Object*)obj)->getClass(obj)) {
    case INTEGER_CLASS:
      if (mpz_cmp(_i(self), _i(obj)) == 0) {
        return 1;
      }
      break;
    case DECIMAL_CLASS:
      if (mpf_integer_p(_d(obj))) {
        mpz_t tmp;
        mpz_init(tmp);
        mpz_set_f(tmp, _d(obj));
        int rval = (mpz_cmp(_i(self), tmp) == 0);
        mpz_clear(tmp);
        return rval;
      }
      break;
    case RATIO_CLASS:
      {
        mpq_t tmp;
        mpq_init(tmp);
        mpq_set_z(tmp, _i(self));
        int rval = mpq_equal(tmp, _r(obj));
        mpq_clear(tmp);
        return rval;
      }
    }
  }
  return 0;
}

Number IntegerProto = {
  .init = Integer_init,
  .destroy = Integer_destroy,
  .getClass = Integer_getClass,
  .toString = Integer_toString,
  .instanceOf = Integer_instanceOf,
  .equals = Integer_equals
};

Integer *Integer_new() {
  _super_Number_new(Integer, obj);
  return obj;
}

Integer *Integer_valueOf(const UChar *s, int radix) {
  _super_Number_new(Integer, obj);
  char *astr = malloc(u_strlen(s)+1);
  u_austrcpy(astr, s);

  mpz_t *it = malloc(sizeof(mpz_t));
  mpz_init(*it);
  if (mpz_set_str(*it, astr, radix) != 0) {
    free(astr);
    obj->destroy(obj);
    return NULL;
  }
  free(astr);
  obj->_num = it;
  return obj;
}
