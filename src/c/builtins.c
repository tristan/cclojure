/*
  Extra femtoLisp builtin functions
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include "llt.h"
#include "clojure.h"
#include "random.h"

size_t llength(value_t v)
{
    size_t n = 0;
    while (iscons(v)) {
        n++;
        v = cdr_(v);
    }
    return n;
}

static value_t clj_nconc(value_t *args, u_int32_t nargs)
{
    if (nargs == 0)
        return CLJ_NIL;
    value_t lst, first=CLJ_NIL;
    value_t *pcdr = &first;
    cons_t *c;
    uint32_t i=0;
    while (1) {
        lst = args[i++];
        if (i >= nargs) break;
        if (iscons(lst)) {
            *pcdr = lst;
            c = (cons_t*)ptr(lst);
            while (iscons(c->cdr))
                c = (cons_t*)ptr(c->cdr);
            pcdr = &c->cdr;
        }
        else if (lst != CLJ_NIL) {
            type_error("nconc", "cons", lst);
        }
    }
    *pcdr = lst;
    return first;
}

static value_t clj_assq(value_t *args, u_int32_t nargs)
{
    argcount("assq", nargs, 2);
    value_t item = args[0];
    value_t v = args[1];
    value_t bind;

    while (iscons(v)) {
        bind = car_(v);
        if (iscons(bind) && car_(bind) == item)
            return bind;
        v = cdr_(v);
    }
    return CLJ_F;
}

static value_t clj_memq(value_t *args, u_int32_t nargs)
{
    argcount("memq", nargs, 2);
    while (iscons(args[1])) {
        cons_t *c = (cons_t*)ptr(args[1]);
        if (c->car == args[0])
            return args[1];
        args[1] = c->cdr;
    }
    return CLJ_F;
}

static value_t clj_length(value_t *args, u_int32_t nargs)
{
    argcount("length", nargs, 1);
    value_t a = args[0];
    cvalue_t *cv;
    if (isvector(a)) {
        return fixnum(vector_size(a));
    }
    else if (iscprim(a)) {
        cv = (cvalue_t*)ptr(a);
        if (cp_class(cv) == bytetype)
            return fixnum(1);
        else if (cp_class(cv) == wchartype)
            return fixnum(u8_charlen(*(uint32_t*)cp_data((cprim_t*)cv)));
    }
    else if (iscvalue(a)) {
        cv = (cvalue_t*)ptr(a);
        if (cv_class(cv)->eltype != NULL)
            return size_wrap(cvalue_arraylen(a));
    }
    else if (a == CLJ_NIL) {
        return fixnum(0);
    }
    else if (iscons(a)) {
        return fixnum(llength(a));
    }
    type_error("length", "sequence", a);
}

static value_t clj_f_raise(value_t *args, u_int32_t nargs)
{
    argcount("raise", nargs, 1);
    clj_raise(args[0]);
}

static value_t clj_exit(value_t *args, u_int32_t nargs)
{
    if (nargs > 0)
        exit(tofixnum(args[0], "exit"));
    exit(0);
    return CLJ_NIL;
}

static value_t clj_symbol(value_t *args, u_int32_t nargs)
{
    argcount("symbol", nargs, 1);
    if (!clj_isstring(args[0]))
        type_error("symbol", "string", args[0]);
    return symbol(cvalue_data(args[0]));
}

static value_t clj_keywordp(value_t *args, u_int32_t nargs)
{
    argcount("keyword?", nargs, 1);
    return (issymbol(args[0]) &&
            iskeyword((symbol_t*)ptr(args[0]))) ? CLJ_T : CLJ_F;
}

static value_t clj_top_level_value(value_t *args, u_int32_t nargs)
{
    argcount("top-level-value", nargs, 1);
    symbol_t *sym = tosymbol(args[0], "top-level-value");
    if (sym->binding == UNBOUND)
        clj_raise(clj_list2(UnboundError, args[0]));
    return sym->binding;
}

static value_t clj_set_top_level_value(value_t *args, u_int32_t nargs)
{
    argcount("set-top-level-value!", nargs, 2);
    symbol_t *sym = tosymbol(args[0], "set-top-level-value!");
    if (!isconstant(sym))
        sym->binding = args[1];
    return args[1];
}

static void global_env_list(symbol_t *root, value_t *pv)
{
    while (root != NULL) {
        if (root->name[0] != ':' && (root->binding != UNBOUND)) {
            *pv = clj_cons(tagptr(root,TAG_SYM), *pv);
        }
        global_env_list(root->left, pv);
        root = root->right;
    }
}

extern symbol_t *symtab;

value_t clj_global_env(value_t *args, u_int32_t nargs)
{
    (void)args;
    argcount("environment", nargs, 0);
    value_t lst = CLJ_NIL;
    clj_gc_handle(&lst);
    global_env_list(symtab, &lst);
    clj_free_gc_handles(1);
    return lst;
}

extern value_t QUOTE;

static value_t clj_constantp(value_t *args, u_int32_t nargs)
{
    argcount("constant?", nargs, 1);
    if (issymbol(args[0]))
        return (isconstant((symbol_t*)ptr(args[0])) ? CLJ_T : CLJ_F);
    if (iscons(args[0])) {
        if (car_(args[0]) == QUOTE)
            return CLJ_T;
        return CLJ_F;
    }
    return CLJ_T;
}

static value_t clj_integer_valuedp(value_t *args, u_int32_t nargs)
{
    argcount("integer-valued?", nargs, 1);
    value_t v = args[0];
    if (isfixnum(v)) {
        return CLJ_T;
    }
    else if (iscprim(v)) {
        numerictype_t nt = cp_numtype((cprim_t*)ptr(v));
        if (nt < T_FLOAT)
            return CLJ_T;
        void *data = cp_data((cprim_t*)ptr(v));
        if (nt == T_FLOAT) {
            float f = *(float*)data;
            if (f < 0) f = -f;
            if (f <= FLT_MAXINT && (float)(int32_t)f == f)
                return CLJ_T;
        }
        else {
            assert(nt == T_DOUBLE);
            double d = *(double*)data;
            if (d < 0) d = -d;
            if (d <= DBL_MAXINT && (double)(int64_t)d == d)
                return CLJ_T;
        }
    }
    return CLJ_F;
}

static value_t clj_integerp(value_t *args, u_int32_t nargs)
{
    argcount("integer?", nargs, 1);
    value_t v = args[0];
    return (isfixnum(v) ||
            (iscprim(v) && cp_numtype((cprim_t*)ptr(v)) < T_FLOAT)) ?
        CLJ_T : CLJ_F;
}

static value_t clj_fixnum(value_t *args, u_int32_t nargs)
{
    argcount("fixnum", nargs, 1);
    if (isfixnum(args[0])) {
        return args[0];
    }
    else if (iscprim(args[0])) {
        cprim_t *cp = (cprim_t*)ptr(args[0]);
        return fixnum(conv_to_long(cp_data(cp), cp_numtype(cp)));
    }
    type_error("fixnum", "number", args[0]);
}

static value_t clj_truncate(value_t *args, u_int32_t nargs)
{
    argcount("truncate", nargs, 1);
    if (isfixnum(args[0]))
        return args[0];
    if (iscprim(args[0])) {
        cprim_t *cp = (cprim_t*)ptr(args[0]);
        void *data = cp_data(cp);
        numerictype_t nt = cp_numtype(cp);
        double d;
        if (nt == T_FLOAT)
            d = (double)*(float*)data;
        else if (nt == T_DOUBLE)
            d = *(double*)data;
        else
            return args[0];
        if (d > 0) {
            if (d > (double)U64_MAX)
                return args[0];
            return return_from_uint64((uint64_t)d);
        }
        if (d > (double)S64_MAX || d < (double)S64_MIN)
            return args[0];
        return return_from_int64((int64_t)d);
    }
    type_error("truncate", "number", args[0]);
}

static value_t clj_vector_alloc(value_t *args, u_int32_t nargs)
{
    fixnum_t i;
    value_t f, v;
    if (nargs == 0)
        lerror(ArgError, "vector.alloc: too few arguments");
    i = (fixnum_t)toulong(args[0], "vector.alloc");
    if (i < 0)
        lerror(ArgError, "vector.alloc: invalid size");
    if (nargs == 2)
        f = args[1];
    else
        f = CLJ_UNSPECIFIED;
    v = alloc_vector((unsigned)i, f==CLJ_UNSPECIFIED);
    if (f != CLJ_UNSPECIFIED) {
        int k;
        for(k=0; k < i; k++)
            vector_elt(v,k) = f;
    }
    return v;
}

static value_t clj_time_now(value_t *args, u_int32_t nargs)
{
    argcount("time.now", nargs, 0);
    (void)args;
    return mk_double(clock_now());
}

static double todouble(value_t a, char *fname)
{
    if (isfixnum(a))
        return (double)numval(a);
    if (iscprim(a)) {
        cprim_t *cp = (cprim_t*)ptr(a);
        numerictype_t nt = cp_numtype(cp);
        return conv_to_double(cp_data(cp), nt);
    }
    type_error(fname, "number", a);
}

static value_t clj_time_string(value_t *args, uint32_t nargs)
{
    argcount("time.string", nargs, 1);
    double t = todouble(args[0], "time.string");
    char buf[64];
    timestring(t, buf, sizeof(buf));
    return string_from_cstr(buf);
}

static value_t clj_time_fromstring(value_t *args, uint32_t nargs)
{
    argcount("time.fromstring", nargs, 1);
    char *ptr = tostring(args[0], "time.fromstring");
    double t = parsetime(ptr);
    int64_t it = (int64_t)t;
    if ((double)it == t && fits_fixnum(it))
        return fixnum(it);
    return mk_double(t);
}

static value_t clj_path_cwd(value_t *args, uint32_t nargs)
{
    if (nargs > 1)
        argcount("path.cwd", nargs, 1);
    if (nargs == 0) {
        char buf[1024];
        get_cwd(buf, sizeof(buf));
        return string_from_cstr(buf);
    }
    char *ptr = tostring(args[0], "path.cwd");
    if (set_cwd(ptr))
        lerrorf(IOError, "path.cwd: could not cd to %s", ptr);
    return CLJ_T;
}

#ifdef WIN32
#define stat _stat
#endif
static value_t clj_path_exists(value_t *args, uint32_t nargs)
{
    argcount("path.exists?", nargs, 1);
    char *str = tostring(args[0], "path.exists?");
    struct stat sbuf;
    if (stat(str, &sbuf) == -1)
        return CLJ_F;
    return CLJ_T;
}

static value_t clj_os_getenv(value_t *args, uint32_t nargs)
{
    argcount("os.getenv", nargs, 1);
    char *name = tostring(args[0], "os.getenv");
    char *val = getenv(name);
    if (val == NULL) return CLJ_F;
    if (*val == 0)
        return symbol_value(emptystringsym);
    return cvalue_static_cstring(val);
}

static value_t clj_os_setenv(value_t *args, uint32_t nargs)
{
    argcount("os.setenv", nargs, 2);
    char *name = tostring(args[0], "os.setenv");
    int result;
    if (args[1] == CLJ_F) {
#ifdef LINUX
        result = unsetenv(name);
#else
        (void)unsetenv(name);
        result = 0;
#endif
    }
    else {
        char *val = tostring(args[1], "os.setenv");
        result = setenv(name, val, 1);
    }
    if (result != 0)
        lerror(ArgError, "os.setenv: invalid environment variable");
    return CLJ_T;
}

static value_t clj_rand(value_t *args, u_int32_t nargs)
{
    (void)args; (void)nargs;
    fixnum_t r;
#ifdef BITS64
    r = ((((uint64_t)random())<<32) | random()) & 0x1fffffffffffffffLL;
#else
    r = random() & 0x1fffffff;
#endif
    return fixnum(r);
}
static value_t clj_rand32(value_t *args, u_int32_t nargs)
{
    (void)args; (void)nargs;
    uint32_t r = random();
#ifdef BITS64
    return fixnum(r);
#else
    return mk_uint32(r);
#endif
}
static value_t clj_rand64(value_t *args, u_int32_t nargs)
{
    (void)args; (void)nargs;
    uint64_t r = (((uint64_t)random())<<32) | random();
    return mk_uint64(r);
}
static value_t clj_randd(value_t *args, u_int32_t nargs)
{
    (void)args; (void)nargs;
    return mk_double(rand_double());
}
static value_t clj_randf(value_t *args, u_int32_t nargs)
{
    (void)args; (void)nargs;
    return mk_float(rand_float());
}

extern void stringfuncs_init();
extern void table_init();
extern void iostream_init();

static builtinspec_t builtin_info[] = {
    { "environment", clj_global_env },
    { "constant?", clj_constantp },
    { "top-level-value", clj_top_level_value },
    { "set-top-level-value!", clj_set_top_level_value },
    { "raise", clj_f_raise },
    { "exit", clj_exit },
    { "symbol", clj_symbol },
    { "keyword?", clj_keywordp },

    { "fixnum", clj_fixnum },
    { "truncate", clj_truncate },
    { "integer?", clj_integerp },
    { "integer-valued?", clj_integer_valuedp },
    { "nconc", clj_nconc },
    { "append!", clj_nconc },
    { "assq", clj_assq },
    { "memq", clj_memq },
    { "length", clj_length },

    { "vector.alloc", clj_vector_alloc },

    { "time.now", clj_time_now },
    { "time.string", clj_time_string },
    { "time.fromstring", clj_time_fromstring },

    { "rand", clj_rand },
    { "rand.uint32", clj_rand32 },
    { "rand.uint64", clj_rand64 },
    { "rand.double", clj_randd },
    { "rand.float", clj_randf },

    { "path.cwd", clj_path_cwd },
    { "path.exists?", clj_path_exists },

    { "os.getenv", clj_os_getenv },
    { "os.setenv", clj_os_setenv },
    { NULL, NULL }
};

void builtins_init()
{
    assign_global_builtins(builtin_info);
    stringfuncs_init();
    table_init();
    iostream_init();
}
