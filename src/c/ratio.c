#include <string.h>
#include <unicode/ustring.h>
#include <gmp.h>
#include "ratio.h"

#define _i(N) (mpz_t*)((Integer*)N)->_num
#define _r(N) *(mpq_t*)((Ratio*)N)->_num

int Ratio_init(void *self, void *numden) {

  // TODO: are these copies now? do i have to clear them?
  // does this even work?! time to write some tests!
  mpz_t *num = _i(numden);
  mpz_t *den = _i((numden+1));

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

Number RatioProto = {
  .init = Ratio_init,
  .destroy = Ratio_destroy,
  .getClass = Ratio_getClass,
  .toString = Ratio_toString,
  .instanceOf = Ratio_instanceOf,
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
