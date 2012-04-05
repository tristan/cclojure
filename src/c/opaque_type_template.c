#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include "llt.h"
#include "clojure.h"

// global replace TYPE with your type name to make your very own type!

static value_t TYPEsym;
static cljtype_t *TYPEtype;

void print_TYPE(value_t v, ios_t *f, int princ)
{
}

void print_traverse_TYPE(value_t self)
{
}

void free_TYPE(value_t self)
{
}

void relocate_TYPE(value_t oldv, value_t newv)
{
}

cvtable_t TYPE_vtable = { print_TYPE, relocate_TYPE, free_TYPE,
                          print_traverse_TYPE };

int isTYPE(value_t v)
{
    return iscvalue(v) && cv_class((cvalue_t*)ptr(v)) == TYPEtype;
}

value_t clj_TYPEp(value_t *args, uint32_t nargs)
{
    argcount("TYPE?", nargs, 1);
    return isTYPE(args[0]) ? CLJ_T : CLJ_F;
}

static TYPE_t *toTYPE(value_t v, char *fname)
{
    if (!isTYPE(v))
        type_error(fname, "TYPE", v);
    return (TYPE_t*)cv_data((cvalue_t*)ptr(v));
}

static builtinspec_t TYPEfunc_info[] = {
    { "TYPE?", clj_TYPEp },
    { NULL, NULL }
};

void TYPE_init()
{
    TYPEsym = symbol("TYPE");
    TYPEtype = define_opaque_type(TYPEsym, sizeof(TYPE_t),
                                  &TYPE_vtable, NULL);
    assign_global_builtins(TYPEfunc_info);
}
