#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <wctype.h>
#include <sys/types.h>
#include <locale.h>
#include <limits.h>
#include <errno.h>
#include <math.h>
#include "llt.h"
#include "clojure.h"
#include "opcodes.h"

static value_t argv_list(int argc, char *argv[])
{
    int i;
    value_t lst=CLJ_NIL, temp;
    clj_gc_handle(&lst);
    clj_gc_handle(&temp);
    for(i=argc-1; i >= 0; i--) {
        temp = cvalue_static_cstring(argv[i]);
        lst = clj_cons(temp, lst);
    }
    clj_free_gc_handles(2);
    return lst;
}

extern value_t clj_file(value_t *args, uint32_t nargs);

int main(int argc, char *argv[])
{
    char fname_buf[1024];

    clj_init(512*1024);

    fname_buf[0] = '\0';
    value_t str = symbol_value(symbol("*install-dir*"));
    char *exedir = (str == UNBOUND ? NULL : cvalue_data(str));
    if (exedir != NULL) {
        strcat(fname_buf, exedir);
        strcat(fname_buf, PATHSEPSTRING);
    }
    strcat(fname_buf, "src/c/flisp.boot");

    value_t args[2];
    clj_gc_handle(&args[0]);
    clj_gc_handle(&args[1]);
    CLJ_TRY_EXTERN {
        args[0] = cvalue_static_cstring(fname_buf);
        args[1] = symbol(":read");
        value_t f = clj_file(&args[0], 2);
        clj_free_gc_handles(2);

        if (clj_load_system_image(f))
            return 1;

        (void)clj_applyn(1, symbol_value(symbol("__start")),
                         argv_list(argc, argv));
    }
    CLJ_CATCH_EXTERN {
        ios_puts("fatal error:\n", ios_stderr);
        clj_print(ios_stderr, clj_lasterror);
        ios_putc('\n', ios_stderr);
        return 1;
    }
    return 0;
}
