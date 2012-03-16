#include <string.h>
#include <unicode/ustring.h>
#include <gmp.h>
#include "numbers.h"

#define _ip(N) (mpz_t*)((Integer*)N)->_num
#define _i(N) *(mpz_t*)((Integer*)N)->_num
#define _d(N) *(mpf_t*)((Decimal*)N)->_num
#define _r(N) *(mpq_t*)((Ratio*)N)->_num

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
  .destroy = Ratio_destroy,
  .getClass = Ratio_getClass,
  .toString = Ratio_toString,
  .instanceOf = Ratio_instanceOf,
  .equals = Ratio_equals
};

Ratio *Ratio_new_i(int numerator, int denominator) {
  _super_Number_new(Ratio, obj);

  mpq_t* rt = malloc(sizeof(mpq_t));
  mpq_init(*rt);
  mpq_set_si(*rt, numerator, denominator);

  obj->_num = rt;
  return obj;
}

Ratio *Ratio_new_I(Integer* numerator, Integer* denominator) {
  _super_Number_new(Ratio, obj);

  mpq_t* rt = malloc(sizeof(mpq_t));
  mpq_init(*rt);
  mpq_set_num(*rt, _i(numerator));
  mpq_set_den(*rt, _i(denominator));

  obj->_num = rt;
  return obj;
}

Ratio *Ratio_new_s(const UChar *numerator, const UChar *denominator) {
  _super_Number_new(Ratio, obj);

  char *adivid = malloc(u_strlen(numerator)+1);
  char *adivis = malloc(u_strlen(denominator)+1);
  u_austrcpy(adivid, numerator);
  u_austrcpy(adivis, denominator);

  mpq_t num;
  mpq_t den;
  mpq_inits(num, den, NULL);
  if (mpq_set_str(num, adivid, 10) != 0) return NULL;
  if (mpq_set_str(den, adivis, 10) != 0) return NULL;

  mpq_t *quad = malloc(sizeof(mpq_t));
  mpq_init(*quad);
  mpq_div(*quad, num, den);

  mpq_clear(num);
  mpq_clear(den);

  free(adivid);
  free(adivis);

  obj->_num = quad;
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
