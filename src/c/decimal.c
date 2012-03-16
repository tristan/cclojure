#include <string.h>
#include <unicode/ustring.h>
#include <gmp.h>
#include "numbers.h"

#define _i(N) *(mpz_t*)((Integer*)N)->_num
#define _d(N) *(mpf_t*)((Decimal*)N)->_num
#define _r(N) *(mpq_t*)((Ratio*)N)->_num

void Decimal_destroy(void *self) {
  if (((Decimal*)self)->_num != NULL) {
    mpf_clear(((Decimal*)self)->_num);
  }
  free(self);
}

int Decimal_instanceOf(void *self, int class) {
  return class == DECIMAL_CLASS || Number_instanceOf(self, class);
}

int Decimal_getClass(void *self) {
  return DECIMAL_CLASS;
}

UChar *Decimal_toString(void *self) {
  mp_exp_t expptr;
  char *astr = mpf_get_str(NULL, &expptr, 10, 0, _d(self));
  int len = strlen(astr);
  if (len > expptr) {
    char *tmp = malloc(len+2);
    strncpy(tmp, astr, expptr);
    tmp[expptr] = '.';
    strcpy(tmp+expptr+1, astr+expptr);
    free(astr);
    astr = tmp;
  }
  if (astr) {
    UChar *ustr = malloc(sizeof(UChar) * strlen(astr));
    u_uastrcpy(ustr, astr);
    free(astr);
    return ustr;
  }
  return NULL;
}

int Decimal_equals(void *self, void* obj) {
  if (((Object*)self)->getClass(self) != Decimal_getClass(self)) {
    puts("calling Decimal_equals on non decimal object is strange!");
  } else {
    switch (((Object*)obj)->getClass(obj)) {
    case INTEGER_CLASS:
      if (mpf_integer_p(_d(self))) {
        mpz_t tmp;
        mpz_init(tmp);
        mpz_set_f(tmp, _d(self));
        int rval = (mpz_cmp(tmp, _i(obj)) == 0);
        mpz_clear(tmp);
        return rval;
      }
      break;
    case DECIMAL_CLASS:
      if (mpf_cmp(_d(self), _d(obj)) == 0) {
        return 1;
      }
      break;
    case RATIO_CLASS:
      {
        mpf_t tmp;
        mpf_init(tmp);
        mpf_set_q(tmp, _r(obj));
        int rval = (mpf_cmp(_d(self), tmp) == 0);
        mpf_clear(tmp);
        return rval;
      }
    }
  }
  return 0;
}

Number DecimalProto = {
  .destroy = Decimal_destroy,
  .getClass = Decimal_getClass,
  .toString = Decimal_toString,
  .instanceOf = Decimal_instanceOf,
  .equals = Decimal_equals
};

Decimal *Decimal_new(double num) {
  _super_Number_new(Decimal, obj);
  mpf_t* dt = malloc(sizeof(mpf_t));
  mpf_init(*dt);
  mpf_set_d(*dt, num);

  obj->_num = dt;
  return obj;
}

Decimal *Decimal_valueOf(const UChar *s) {
  _super_Number_new(Decimal, obj);
  char *astr = malloc(u_strlen(s)+1);
  u_austrcpy(astr, s);

  mpf_t *dt = malloc(sizeof(mpf_t));
  mpf_init(*dt);
  if (mpf_set_str(*dt, astr, 10) != 0) {
    free(astr);
    obj->destroy(obj);
    return NULL;
  }
  free(astr);
  obj->_num = dt;
  return obj;
}
