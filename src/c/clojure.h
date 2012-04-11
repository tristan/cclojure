#ifndef _CLOJURE_H
#define _CLOJURE_H

typedef uptrint_t value_t;

void lerror(jmp_buf toplevel, char *format, ...);

#endif
