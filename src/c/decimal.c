#include <string.h>
#include <unicode/ustring.h>
#include <gmp.h>
#include "decimal.h"

#define _d(N) *(mpf_t*)((Decimal*)N)->_num

int Decimal_init(void *self, void *num) {
  mpf_t* dt = malloc(sizeof(mpf_t));
  mpf_init(*dt);
  mpf_set_d(*dt, *(double*)num);

  ((Decimal*)self)->_num = dt;
  return 1;
}

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

Number DecimalProto = {
  .init = Decimal_init,
  .destroy = Decimal_destroy,
  .getClass = Decimal_getClass,
  .toString = Decimal_toString,
  .instanceOf = Decimal_instanceOf,
};

Decimal *Decimal_new() {
  _super_Number_new(Decimal, obj);
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
