#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <setjmp.h>
#include "llt.h"
#include "clojure.h"

static value_t iostreamsym, rdsym, wrsym, apsym, crsym, truncsym;
static value_t instrsym, outstrsym;
cljtype_t *iostreamtype;

void print_iostream(value_t v, ios_t *f)
{
    (void)v;
    clj_print_str("#<io stream>", f);
}

void free_iostream(value_t self)
{
    ios_t *s = value2c(ios_t*, self);
    ios_close(s);
}

void relocate_iostream(value_t oldv, value_t newv)
{
    ios_t *olds = value2c(ios_t*, oldv);
    ios_t *news = value2c(ios_t*, newv);
    cvalue_t *cv = (cvalue_t*)ptr(oldv);
    if (isinlined(cv)) {
        if (olds->buf == &olds->local[0]) {
            news->buf = &news->local[0];
        }
    }
}

cvtable_t iostream_vtable = { print_iostream, relocate_iostream,
                              free_iostream, NULL };

int clj_isiostream(value_t v)
{
    return iscvalue(v) && cv_class((cvalue_t*)ptr(v)) == iostreamtype;
}

value_t clj_iostreamp(value_t *args, uint32_t nargs)
{
    argcount("iostream?", nargs, 1);
    return clj_isiostream(args[0]) ? CLJ_T : CLJ_F;
}

value_t clj_eof_object(value_t *args, uint32_t nargs)
{
    (void)args;
    argcount("eof-object", nargs, 0);
    return CLJ_EOF;
}

value_t clj_eof_objectp(value_t *args, uint32_t nargs)
{
    argcount("eof-object?", nargs, 1);
    return (CLJ_EOF == args[0]) ? CLJ_T : CLJ_F;
}

static ios_t *toiostream(value_t v, char *fname)
{
    if (!clj_isiostream(v))
        type_error(fname, "iostream", v);
    return value2c(ios_t*, v);
}

ios_t *clj_toiostream(value_t v, char *fname)
{
    return toiostream(v, fname);
}

value_t clj_file(value_t *args, uint32_t nargs)
{
    if (nargs < 1)
        argcount("file", nargs, 1);
    int i, r=0, w=0, c=0, t=0, a=0;
    for(i=1; i < (int)nargs; i++) {
        if      (args[i] == wrsym)    w = 1;
        else if (args[i] == apsym)    { a = 1; w = 1; }
        else if (args[i] == crsym)    { c = 1; w = 1; }
        else if (args[i] == truncsym) { t = 1; w = 1; }
        else if (args[i] == rdsym)    r = 1;
    }
    if ((r|w|c|t|a) == 0) r = 1;  // default to reading
    value_t f = cvalue(iostreamtype, sizeof(ios_t));
    char *fname = tostring(args[0], "file");
    ios_t *s = value2c(ios_t*, f);
    if (ios_file(s, fname, r, w, c, t) == NULL)
        lerrorf(IOError, "file: could not open \"%s\"", fname);
    if (a) ios_seek_end(s);
    return f;
}

value_t clj_buffer(value_t *args, u_int32_t nargs)
{
    argcount("buffer", nargs, 0);
    (void)args;
    value_t f = cvalue(iostreamtype, sizeof(ios_t));
    ios_t *s = value2c(ios_t*, f);
    if (ios_mem(s, 0) == NULL)
        lerror(MemoryError, "buffer: could not allocate stream");
    return f;
}

value_t clj_read(value_t *args, u_int32_t nargs)
{
    value_t arg;
    if (nargs > 1) {
        argcount("read", nargs, 1);
    }
    else if (nargs == 0) {
        arg = symbol_value(instrsym);
    }
    else {
        arg = args[0];
    }
    (void)toiostream(arg, "read");
    clj_gc_handle(&arg);
    value_t v = clj_read_sexpr(arg);
    clj_free_gc_handles(1);
    if (ios_eof(value2c(ios_t*,arg)))
        return CLJ_EOF;
    return v;
}

value_t clj_iogetc(value_t *args, u_int32_t nargs)
{
    argcount("io.getc", nargs, 1);
    ios_t *s = toiostream(args[0], "io.getc");
    uint32_t wc;
    if (ios_getutf8(s, &wc) == IOS_EOF)
        //lerror(IOError, "io.getc: end of file reached");
        return CLJ_EOF;
    return mk_wchar(wc);
}

value_t clj_iopeekc(value_t *args, u_int32_t nargs)
{
    argcount("io.peekc", nargs, 1);
    ios_t *s = toiostream(args[0], "io.peekc");
    uint32_t wc;
    if (ios_peekutf8(s, &wc) == IOS_EOF)
        return CLJ_EOF;
    return mk_wchar(wc);
}

value_t clj_ioputc(value_t *args, u_int32_t nargs)
{
    argcount("io.putc", nargs, 2);
    ios_t *s = toiostream(args[0], "io.putc");
    if (!iscprim(args[1]) || ((cprim_t*)ptr(args[1]))->type != wchartype)
        type_error("io.putc", "wchar", args[1]);
    uint32_t wc = *(uint32_t*)cp_data((cprim_t*)ptr(args[1]));
    return fixnum(ios_pututf8(s, wc));
}

value_t clj_ioflush(value_t *args, u_int32_t nargs)
{
    argcount("io.flush", nargs, 1);
    ios_t *s = toiostream(args[0], "io.flush");
    if (ios_flush(s) != 0)
        return CLJ_F;
    return CLJ_T;
}

value_t clj_ioclose(value_t *args, u_int32_t nargs)
{
    argcount("io.close", nargs, 1);
    ios_t *s = toiostream(args[0], "io.close");
    ios_close(s);
    return CLJ_T;
}

value_t clj_iopurge(value_t *args, u_int32_t nargs)
{
    argcount("io.discardbuffer", nargs, 1);
    ios_t *s = toiostream(args[0], "io.discardbuffer");
    ios_purge(s);
    return CLJ_T;
}

value_t clj_ioeof(value_t *args, u_int32_t nargs)
{
    argcount("io.eof?", nargs, 1);
    ios_t *s = toiostream(args[0], "io.eof?");
    return (ios_eof(s) ? CLJ_T : CLJ_F);
}

value_t clj_ioseek(value_t *args, u_int32_t nargs)
{
    argcount("io.seek", nargs, 2);
    ios_t *s = toiostream(args[0], "io.seek");
    size_t pos = toulong(args[1], "io.seek");
    off_t res = ios_seek(s, (off_t)pos);
    if (res == -1)
        return CLJ_F;
    return CLJ_T;
}

value_t clj_iopos(value_t *args, u_int32_t nargs)
{
    argcount("io.pos", nargs, 1);
    ios_t *s = toiostream(args[0], "io.pos");
    off_t res = ios_pos(s);
    if (res == -1)
        return CLJ_F;
    return size_wrap((size_t)res);
}

value_t clj_write(value_t *args, u_int32_t nargs)
{
    if (nargs < 1 || nargs > 2)
        argcount("write", nargs, 1);
    ios_t *s;
    if (nargs == 2)
        s = toiostream(args[1], "write");
    else
        s = toiostream(symbol_value(outstrsym), "write");
    clj_print(s, args[0]);
    return args[0];
}

value_t clj_ioread(value_t *args, u_int32_t nargs)
{
    if (nargs != 3)
        argcount("io.read", nargs, 2);
    (void)toiostream(args[0], "io.read");
    size_t n;
    cljtype_t *ft;
    if (nargs == 3) {
        // form (io.read s type count)
        ft = get_array_type(args[1]);
        n = toulong(args[2], "io.read") * ft->elsz;
    }
    else {
        ft = get_type(args[1]);
        if (ft->eltype != NULL && !iscons(cdr_(cdr_(args[1]))))
            lerror(ArgError, "io.read: incomplete type");
        n = ft->size;
    }
    value_t cv = cvalue(ft, n);
    char *data;
    if (iscvalue(cv)) data = cv_data((cvalue_t*)ptr(cv));
    else data = cp_data((cprim_t*)ptr(cv));
    size_t got = ios_read(value2c(ios_t*,args[0]), data, n);
    if (got < n)
        //lerror(IOError, "io.read: end of input reached");
        return CLJ_EOF;
    return cv;
}

// args must contain data[, offset[, count]]
static void get_start_count_args(value_t *args, uint32_t nargs, size_t sz,
                                 size_t *offs, size_t *nb, char *fname)
{
    if (nargs > 1) {
        *offs = toulong(args[1], fname);
        if (nargs > 2)
            *nb = toulong(args[2], fname);
        else
            *nb = sz - *offs;
        if (*offs >= sz || *offs + *nb > sz)
            bounds_error(fname, args[0], args[1]);
    }
}

value_t clj_iowrite(value_t *args, u_int32_t nargs)
{
    if (nargs < 2 || nargs > 4)
        argcount("io.write", nargs, 2);
    ios_t *s = toiostream(args[0], "io.write");
    if (iscprim(args[1]) && ((cprim_t*)ptr(args[1]))->type == wchartype) {
        if (nargs > 2)
            lerror(ArgError,
                   "io.write: offset argument not supported for characters");
        uint32_t wc = *(uint32_t*)cp_data((cprim_t*)ptr(args[1]));
        return fixnum(ios_pututf8(s, wc));
    }
    char *data;
    size_t sz, offs=0;
    to_sized_ptr(args[1], "io.write", &data, &sz);
    size_t nb = sz;
    if (nargs > 2) {
        get_start_count_args(&args[1], nargs-1, sz, &offs, &nb, "io.write");
        data += offs;
    }
    return size_wrap(ios_write(s, data, nb));
}

value_t clj_dump(value_t *args, u_int32_t nargs)
{
    if (nargs < 1 || nargs > 3)
        argcount("dump", nargs, 1);
    ios_t *s = toiostream(symbol_value(outstrsym), "dump");
    char *data;
    size_t sz, offs=0;
    to_sized_ptr(args[0], "dump", &data, &sz);
    size_t nb = sz;
    if (nargs > 1) {
        get_start_count_args(args, nargs, sz, &offs, &nb, "dump");
        data += offs;
    }
    hexdump(s, data, nb, offs);
    return CLJ_T;
}

static char get_delim_arg(value_t arg, char *fname)
{
    size_t uldelim = toulong(arg, fname);
    if (uldelim > 0x7f) {
        // wchars > 0x7f, or anything else > 0xff, are out of range
        if ((iscprim(arg) && cp_class((cprim_t*)ptr(arg))==wchartype) ||
            uldelim > 0xff)
            lerrorf(ArgError, "%s: delimiter out of range", fname);
    }
    return (char)uldelim;
}

value_t clj_ioreaduntil(value_t *args, u_int32_t nargs)
{
    argcount("io.readuntil", nargs, 2);
    value_t str = cvalue_string(80);
    cvalue_t *cv = (cvalue_t*)ptr(str);
    char *data = cv_data(cv);
    ios_t dest;
    ios_mem(&dest, 0);
    ios_setbuf(&dest, data, 80, 0);
    char delim = get_delim_arg(args[1], "io.readuntil");
    ios_t *src = toiostream(args[0], "io.readuntil");
    size_t n = ios_copyuntil(&dest, src, delim);
    cv->len = n;
    if (dest.buf != data) {
        // outgrew initial space
        cv->data = dest.buf;
#ifndef BOEHM_GC
        cv_autorelease(cv);
#endif
    }
    ((char*)cv->data)[n] = '\0';
    if (n == 0 && ios_eof(src))
        return CLJ_EOF;
    return str;
}

value_t clj_iocopyuntil(value_t *args, u_int32_t nargs)
{
    argcount("io.copyuntil", nargs, 3);
    ios_t *dest = toiostream(args[0], "io.copyuntil");
    ios_t *src = toiostream(args[1], "io.copyuntil");
    char delim = get_delim_arg(args[2], "io.copyuntil");
    return size_wrap(ios_copyuntil(dest, src, delim));
}

value_t clj_iocopy(value_t *args, u_int32_t nargs)
{
    if (nargs < 2 || nargs > 3)
        argcount("io.copy", nargs, 2);
    ios_t *dest = toiostream(args[0], "io.copy");
    ios_t *src = toiostream(args[1], "io.copy");
    if (nargs == 3) {
        size_t n = toulong(args[2], "io.copy");
        return size_wrap(ios_copy(dest, src, n));
    }
    return size_wrap(ios_copyall(dest, src));
}

value_t stream_to_string(value_t *ps)
{
    value_t str;
    size_t n;
    ios_t *st = value2c(ios_t*,*ps);
    if (st->buf == &st->local[0]) {
        n = st->size;
        str = cvalue_string(n);
        memcpy(cvalue_data(str), value2c(ios_t*,*ps)->buf, n);
        ios_trunc(st, 0);
    }
    else {
        char *b = ios_takebuf(st, &n); n--;
        b[n] = '\0';
        str = cvalue_from_ref(stringtype, b, n, CLJ_NIL);
#ifndef BOEHM_GC
        cv_autorelease((cvalue_t*)ptr(str));
#endif
    }
    return str;
}

value_t clj_iotostring(value_t *args, u_int32_t nargs)
{
    argcount("io.tostring!", nargs, 1);
    ios_t *src = toiostream(args[0], "io.tostring!");
    if (src->bm != bm_mem)
        lerror(ArgError, "io.tostring!: requires memory stream");
    return stream_to_string(&args[0]);
}

static builtinspec_t iostreamfunc_info[] = {
    { "iostream?", clj_iostreamp },
    { "eof-object", clj_eof_object },
    { "eof-object?", clj_eof_objectp },
    { "dump", clj_dump },
    { "file", clj_file },
    { "buffer", clj_buffer },
    { "read", clj_read },
    { "write", clj_write },
    { "io.flush", clj_ioflush },
    { "io.close", clj_ioclose },
    { "io.eof?" , clj_ioeof },
    { "io.seek" , clj_ioseek },
    { "io.pos",   clj_iopos },
    { "io.getc" , clj_iogetc },
    { "io.putc" , clj_ioputc },
    { "io.peekc" , clj_iopeekc },
    { "io.discardbuffer", clj_iopurge },
    { "io.read", clj_ioread },
    { "io.write", clj_iowrite },
    { "io.copy", clj_iocopy },
    { "io.readuntil", clj_ioreaduntil },
    { "io.copyuntil", clj_iocopyuntil },
    { "io.tostring!", clj_iotostring },
    { NULL, NULL }
};

void iostream_init()
{
    iostreamsym = symbol("iostream");
    rdsym = symbol(":read");
    wrsym = symbol(":write");
    apsym = symbol(":append");
    crsym = symbol(":create");
    truncsym = symbol(":truncate");
    instrsym = symbol("*input-stream*");
    outstrsym = symbol("*output-stream*");
    iostreamtype = define_opaque_type(iostreamsym, sizeof(ios_t),
                                      &iostream_vtable, NULL);
    assign_global_builtins(iostreamfunc_info);

    setc(symbol("*stdout*"), cvalue_from_ref(iostreamtype, ios_stdout,
                                             sizeof(ios_t), CLJ_NIL));
    setc(symbol("*stderr*"), cvalue_from_ref(iostreamtype, ios_stderr,
                                             sizeof(ios_t), CLJ_NIL));
    setc(symbol("*stdin*" ), cvalue_from_ref(iostreamtype, ios_stdin,
                                             sizeof(ios_t), CLJ_NIL));
}
