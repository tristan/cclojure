#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gmp.h>
#include "numbers.h"

//#define _i(N) ((_data_u*)N->data)
#define _i(N) *(mpz_t*)N->data
#define _f(N) *(mpf_t*)N->data
#define _q(N) *(mpq_t*)N->data

typedef union {
  mpz_t i;
  mpf_t f;
  mpq_t q;
} _data_u;

static Number* build_Number(void *data, int type) {
  Number *rval = malloc(sizeof(Number));
  rval->type = type;
  rval->data = data;

  return rval;
}

Number *Integer_init_int(int i) {
  mpz_t* it = malloc(sizeof(mpz_t));
  mpz_init(*it);
  mpz_set_si(*it, i);

  return build_Number(it, INTEGER);
}

//Number *Integer_init_str(const char *str) {
//  return Integer_init_str(str, 10);
//}

Number *Integer_init_str(const UChar *str, int base) {
  char *astr = malloc(u_strlen(str)+1);
  u_austrcpy(astr, str);

  mpz_t *it = malloc(sizeof(mpz_t));
  mpz_init(*it);
  if (mpz_set_str(*it, astr, base) != 0) {
    free(astr);
    return NULL;
  }

  free(astr);
  return build_Number(it, INTEGER);
}

Number *Float_init_f(float f) {
  mpf_t *ft = malloc(sizeof(mpf_t));
  mpf_init(*ft);
  mpf_set_d(*ft, f);

  return build_Number(ft, FLOAT);
}

Number *Float_init_str(const UChar *str) {
  char *astr = malloc(u_strlen(str)+1);
  u_austrcpy(astr, str);

  mpf_t *ft = malloc(sizeof(mpf_t));
  mpf_init(*ft);
  if (mpf_set_str(*ft, astr, 10) != 0) {
    return NULL;
  }

  free(astr);
  return build_Number(ft, FLOAT);
}

Number *Ratio_init_str(const UChar *dividend, const UChar *divisor) {
  char *adivid = malloc(u_strlen(dividend)+1);
  char *adivis = malloc(u_strlen(divisor)+1);
  u_austrcpy(adivid, dividend);
  u_austrcpy(adivis, divisor);

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

  // TODO: it may be worth checking if this is a whole number
  // and converting it to an integer if it is

  return build_Number(quad, RATIO);
}

void Number_destroy(Number *num) {
  if (num != NULL) {
    if (num->data != NULL) {
      if (num->type == INTEGER) {
        mpz_clear(_i(num));
      } else if (num->type == FLOAT && num->data != NULL) {
        mpf_clear(_f(num));
      } else if (num->type == RATIO && num->data != NULL) {
        mpq_clear(_q(num));
      }
      //free(num->data);
    }
    free(num);
  }
}


UChar *Number_toString(Number *num) {
  if (num == NULL || num->data == NULL) {
    UChar *null = malloc(sizeof(UChar)*4);
    u_uastrcpy(null, "nil");
    return null;
  } else {
    //printf("print: %d %p\n", num->type, num->data);
  }

  char *str = NULL;
  if (num->type == INTEGER) {
    str = mpz_get_str(NULL, 10, _i(num));
  } else if (num->type == FLOAT) {
    mp_exp_t expptr;
    str = mpf_get_str(NULL, &expptr, 10, 0, _f(num));
    int len = strlen(str);
    if (len > expptr) {
      char *tmp = malloc(len+2);
      strncpy(tmp, str, expptr);
      tmp[expptr] = '.';
      strcpy(tmp+expptr+1, str+expptr);
      free(str);
      str = tmp;
    }
  } else if (num->type == RATIO) {
    str = mpq_get_str(NULL, 10, _q(num));
  }
  if (str) {
    UChar *ustr = malloc(sizeof(UChar) * strlen(str));
    u_uastrcpy(ustr, str);
    free(str);
    return ustr;
  }
  return NULL;
}

void Integer_negate(Number *num) {
  mpz_neg(_i(num), _i(num));
}
