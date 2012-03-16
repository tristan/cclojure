#include <string.h>
#include <unicode/ustring.h>
#include <gmp.h>
#include "numbers.h"

#define _ip(N) (mpz_t*)((Integer*)N)->_num
#define _i(N) *(mpz_t*)((Integer*)N)->_num
#define _d(N) *(mpf_t*)((Decimal*)N)->_num
#define _r(N) *(mpq_t*)((Ratio*)N)->_num

int Ratio_init(void *self, void *numden) {

  // TODO: are these copies now? do i have to clear them?
  // does this even work?! time to write some tests!
  mpz_t *num = _ip(numden);
  mpz_t *den = _ip((numden+1));

  mpq_t* rt = malloc(sizeof(mpq_t));
  mpq_init(*rt);
  mpq_set_num(*rt, *num);
  mpq_set_den(*rt, *den);

  ((Ratio*)self)->_num = rt;
  return 1;
}

void Ratio_destroy(void *self) {
  if (((Ratio*)self)->_num != NULL) {
    mpq_clear(((Ratio*)self)->_num);
  }
  free(self);
}

int Ratio_instanceOf(void *self, int class) {
  return class == RATIO_CLASS || Number_instanceOf(self, class);
}

int Ratio_getClass(void *self) {
  return RATIO_CLASS;
}

UChar *Ratio_toString(void *self) {
  char *astr = mpq_get_str(NULL, 10, _r(self));
  if (astr) {
    UChar *ustr = malloc(sizeof(UChar) * strlen(astr));
    u_uastrcpy(ustr, astr);
    free(astr);
    return ustr;
  }
  return NULL;
}

int Ratio_equals(void *self, void* obj) {
  if (((Object*)self)->getClass(self) != Ratio_getClass(self)) {
    puts("calling Ratio_equals on non ratio object is strange!");
  } else {
    switch (((Object*)obj)->getClass(obj)) {
    case INTEGER_CLASS:
      {
        mpq_t tmp;
        mpq_init(tmp);
        mpq_set_z(tmp, _i(obj));
        int rval = mpq_equal(_r(self), tmp);
        mpq_clear(tmp);
        return rval;
      }
    case DECIMAL_CLASS:
      {
        mpf_t tmp;
        mpf_init(tmp);
        mpf_set_q(tmp, _r(self));
        int rval = (mpf_cmp(tmp, _d(obj)) == 0);
        mpf_clear(tmp);
        return rval;
      }
    case RATIO_CLASS:
      {
        return mpq_equal(_r(self), _r(obj));
      }
    }
  }
  return 0;
}

Number RatioProto = {
  .init = Ratio_init,
  .destroy = Ratio_destroy,
  .getClass = Ratio_getClass,
  .toString = Ratio_toString,
  .instanceOf = Ratio_instanceOf,
  .equals = Ratio_equals
};

Ratio *Ratio_new() {
  _super_Number_new(Ratio, obj);
  return obj;
}

Ratio *Ratio_valueOf(const UChar *s) {
  _super_Number_new(Ratio, obj);
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
