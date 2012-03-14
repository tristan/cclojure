#include <stdio.h>
#include "unicode_utils.h"

// TODO: this may belong somewhere else
int32_t u_utoi(UChar *u) {
  int32_t nbr = 0;
  while ((*u >= '0') & (*u <= '9')) {
    nbr = (10*nbr) + (*u++ - '0');
  }
  return nbr;
}

/*
void u_fseek(UFILE* f, long offset, int whence) {
  fseek(u_fgetfile(f), offset, whence);
}
*/
