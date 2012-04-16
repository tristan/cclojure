#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <setjmp.h>
#include "llt.h"
#include "clojure.h"
#include "lispreader.h"

typedef struct {
  ios_t *input; // the input stream
  value_t eof_value; // the value of an eof
  char error_on_eof; // should an error be thrown if eof is reached?
  char recursive; // 

  // stack stuff
#define N_STACK 49152
  value_t stack[N_STACK];
  u_int32_t sp;

  // current namespace
  value_t namespace;

  // setjmp variable
  jmp_buf toplevel;
} readerstate_t;

static value_t NIL;

#define PUSH(s,v) (s->stack[s->sp++] = (v))
#define POP(s)   (s->stack[--s->sp])
#define POPN(s,n) (s->sp-=(n))

value_t handle_unexpected_token(readerstate_t *state, char tok);
value_t read_list(readerstate_t *state, char tok);
value_t read_vector(readerstate_t *state, char tok);
value_t read_map(readerstate_t *state, char tok);
value_t read_comment(readerstate_t *state, char tok);
value_t read_string(readerstate_t *state, char tok);
value_t wrapped_read(readerstate_t *state, char tok);
value_t read_meta(readerstate_t *state, char tok);
value_t read_syntax_quote(readerstate_t *state, char tok);
value_t read_unquote(readerstate_t *state, char tok);
value_t read_char(readerstate_t *state, char tok);
value_t read_arg(readerstate_t *state, char tok);
value_t read_dispatch(readerstate_t *state, char tok);
value_t read_number(readerstate_t *state, char tok);
value_t read_token(readerstate_t *state, char tok);

typedef value_t(*macro_fn)(readerstate_t*, char tok);
static macro_fn macros[128] = { // since the char's we work with are unsigned
  ['('] = read_list,
  [')'] = handle_unexpected_token,
  ['['] = read_vector,
  [']'] = handle_unexpected_token,
  ['{'] = read_map,
  ['}'] = handle_unexpected_token,
  [';'] = read_comment,
  ['"'] = read_string,
  ['\''] = wrapped_read,
  ['@'] = wrapped_read,
  ['^'] = read_meta,
  ['`'] = read_syntax_quote,
  ['~'] = read_unquote,
  ['\\'] = read_char,
  ['%'] = read_arg,
  ['#'] = read_dispatch
};

static inline int iswhitespace(char c) {
  return (c == ' ' || c == ',' || isspace(c)) ? 1 : 0;
}

static inline int ismacro(char c) {
  // TODO: is it garanteed that missing values are NULL ?
  // must check bounds because char is signed, but we could read all 256
  return (c > 0 && c < 128 && macros[(int)c] != NULL) ? 1 : 0;
}

static char nextchar(ios_t *f, char skip_whitespace) {
  int ch;
  char c;
  
  do {
    if (f->bpos < f->size) {
      ch = f->buf[f->bpos++];
    } else {
      ch = ios_getc(f);
      if (ch == IOS_EOF) {
        return IOS_EOF;
      }
    }
    c = (char)ch;
  } while (skip_whitespace && iswhitespace(c));
  return c;
}

static value_t read(readerstate_t *state, char c) {
  if (ios_eof(state->input)) {
    if (state->error_on_eof) {
      lerror(state->toplevel, "EOF while reading");
    }
    return state->eof_value;
  } else {
    if (c == '+' || c == '-') {
      char c2 = nextchar(state->input, 0);
      if (isdigit(c2)) {
        ios_ungetc(c2, state->input);
        return read_number(state, c);
      }
    }
    if (isdigit(c)) {
      value_t r = read_number(state, c);
      assert(r != 0);
      return r;
    }
    if (ismacro(c)) {
      return macros[(int)c](state, c);
    }
    return read_token(state, c);
  }
}

value_t lispreader_read(ios_t *in, value_t ns, char eof_is_error, value_t eof_value, char is_recursive) {

  readerstate_t rs = {
    .input = in,
    .eof_value = eof_value,
    .recursive = is_recursive,
    .error_on_eof = eof_is_error,
    .namespace = ns,
    .sp = 0
  };
  if (setjmp(rs.toplevel)) {
    // an error occured, clean up
    // TODO: should probably free everything that was put on the stack here
    return 0; // TODO: store error in state, return that
  } else {
    if (macros[0] != NULL) { // check to make sure non-specified macros are null
      // can remove this if this is proven to always be the case
      lerror(rs.toplevel, "macros[0] is not null!");
    }
    return read(&rs, nextchar(rs.input, 1));
  }
}

value_t handle_unexpected_token(readerstate_t *state, char tok) {
  lerror(state->toplevel, "Unmatched delimiter: %c", tok);
  return 0;
}

value_t read_list(readerstate_t *state, char tok) {
  //lerror(state->toplevel, "not yet implemented");
  value_t rval = 0;
  int sz = 0;
  PUSH(state, NIL);
  value_t *pc = &state->stack[state->sp-1];
  char t = nextchar(state->input, 1);
  while (t != ')') {
    if (ios_eof(state->input)) {
      lerror(state->toplevel, "EOF while reading");
    }
    value_t c = mk_list();
    lcar_(c) = lcdr_(c) = NIL;
    if (islist(*pc)) {
      lcdr_(*pc) = c;
    } else {
      rval = c;
      // TODO: i don't understand the backrefs/label stuff used in femto here
    }
    *pc = c;
    c = read(state, t);
    lcar_(*pc) = c;
    sz++;
    
    t = nextchar(state->input, 1);
  }
  // assign sizes to support O(1) count of lists
  // TODO: this may not really be needed unless
  // building a non-evaluated list (e.g. '(1 2 3))
  *pc = rval;
  while (sz > 0) {
    lsize(*pc) = sz;
    sz--;
    *pc = lcdr_(*pc);
  }
  (void)POP(state);
  return rval;
}

value_t read_vector(readerstate_t *state, char tok) {
  lerror(state->toplevel, "not yet implemented");
  return 0;
}

value_t read_map(readerstate_t *state, char tok) {
  lerror(state->toplevel, "not yet implemented");
  return 0;
}

value_t read_comment(readerstate_t *state, char tok) {
  lerror(state->toplevel, "not yet implemented");
  return 0;
}

value_t read_string(readerstate_t *state, char tok) {
  char *buf;
  char eseq[10];
  size_t i = 0, sz = 64;
  int c;
  buf = malloc(sz);
  while (1) {
    if (i >= sz-4) { // -4 leaves room for the longest possible utf8 seq
      sz *= 2;
      char *tmp = realloc(buf, sz);
      if (tmp == NULL) {
        free(buf);
        lerror(state->toplevel, "out of memory reading string");
      }
      buf = tmp;
    }
    c = ios_getc(state->input);
    if (c == IOS_EOF) {
      free(buf);
      lerror(state->toplevel, "EOF while reading string");
    }
    if (c == '"') {
      break;
    } else if (c == '\\') {
      c = ios_getc(state->input);
      if (c == IOS_EOF) {
        free(buf);
        lerror(state->toplevel, "EOF while reading string");
      }
      int j = 0;
      if (octal_digit(c)) {
        do {
          eseq[j++] = c;
          c = ios_getc(state->input);
        } while (octal_digit(c) && j < 3 && (c != IOS_EOF));
        if (c != IOS_EOF) {
          ios_ungetc(c, state->input);
        }
        if (j != 3) { // java (and thus clojure) demands that 3 numbers must be present
          free(buf);
          lerror(state->toplevel, "Invalid digit: %c", c);
        }
        eseq[j] = '\0';
        u_int32_t wc = strtol(eseq, NULL, 8);
        i += u8_wc_toutf8(&buf[i], wc);
      } else if (c == 'u') { // NOTE: java only supports utf-16 (i.e. only \u and not \x or \U)
        c = ios_getc(state->input);
        while (hex_digit(c) && j < 4 && c != IOS_EOF) {
          eseq[j++] = c;
          c = ios_getc(state->input);
        }
        if (c != IOS_EOF) {
          ios_ungetc(c, state->input);
        }
        if (j != 4) { // java (and thus clojure) demands that 4 numbers must be present
          free(buf);
          lerror(state->toplevel, "Invalid digit: %c", c);
        }
        eseq[j] = '\0';
        u_int32_t wc = strtol(eseq, NULL, 16);
        i += u8_wc_toutf8(&buf[i], wc);
      } else {
        // all valid java escape sequences
        if (c == 'b') {
          buf[i++] = '\b';
        } else if (c == 'n') {
          buf[i++] = '\n';
        } else if (c == 't') {
          buf[i++] = '\t';
        } else if (c == 'f') {
          buf[i++] = '\f';
        } else if (c == 'r') {
          buf[i++] = '\r';
        } else if (c == '"') {
          buf[i++] = '\'';
        } else if (c == '\\') {
          buf[i++] = '\\';
        } else {
          free(buf);
          lerror(state->toplevel, "Unsupported escape character: \\%c", c);
        }
      }
    } else {
      buf[i++] = c;
    }
  }
  // TODO: make value to return
  buf[i] = '\0';
  value_t r = mk_stringn(buf, i);
  free(buf);
  return r;
}

value_t wrapped_read(readerstate_t *state, char tok) {
  lerror(state->toplevel, "not yet implemented");
  return 0;
}

value_t read_meta(readerstate_t *state, char tok) {
  lerror(state->toplevel, "not yet implemented");
  return 0;
}

value_t read_syntax_quote(readerstate_t *state, char tok) {
  lerror(state->toplevel, "not yet implemented");
  return 0;
}

value_t read_unquote(readerstate_t *state, char tok) {
  lerror(state->toplevel, "not yet implemented");
  return 0;
}

value_t read_char(readerstate_t *state, char tok) {
  lerror(state->toplevel, "not yet implemented");
  return 0;
}

value_t read_arg(readerstate_t *state, char tok) {
  lerror(state->toplevel, "not yet implemented");
  return 0;
}

value_t read_dispatch(readerstate_t *state, char tok) {
  lerror(state->toplevel, "not yet implemented");
  return 0;
}

value_t read_number(readerstate_t *state, char tok) {
  char *buf1 = NULL, *buf2 = NULL, special = 0;
  size_t i = 0, sz = 64;
  int c;
  u_int32_t base = 10;
  buf1 = malloc(sz);
  while (1) {
    if (tok == '0') {
      c = ios_getc(state->input);
      if (c == IOS_EOF || iswhitespace(c)) {
        buf1[0] = tok;
        buf1[1] = '\0';
        value_t rval = mk_integer("0", 10);
        free(buf1);
        return rval;
      } else if (c == 'x') {
        base = 16;
        special = 'x';
        break;
      } else if (c >= '0' && c <= '7') {
        ios_ungetc(c, state->input);
        base = 8;
        special = '0';
        break;
      }
    } else if (tok == '-' || tok == '+') {
      if (tok == '-') {
        buf1[i++] = tok;
      }
      tok = ios_getc(state->input);
    } else {
      buf1[i++] = tok;
      break;
    }
  }
  while (1) {
    if (i >= sz-1) {
      sz *= 2;
      char *tmp = realloc(buf1, sz);
      if (tmp == NULL) {
        free(buf1);
        lerror(state->toplevel, "out of memory reading number");
      }
      buf1 = tmp;
    }
    c = ios_getc(state->input);
    if (c == IOS_EOF || iswhitespace(c) || ismacro(c)) {
      value_t num = 0;
      buf1[i] = '\0';
      if (buf2 != NULL) {
        if (special == '/') {
          num = mk_ratio(buf2, buf1);
        } else {
          assert(0); // TODO: should never get here, remove when decent tests exist
        }
        free(buf2);
      } else {
        if (special == '.' || special == 'e') {
          num = mk_decimal(buf1);
        } else {
          num = mk_integer(buf1, base);
        }
      }
      free(buf1);
      return num;
    }
    if (c == 'r' && i < 3) {
      // check if we're already in a special number form (meaning entering another in invalid)
      if (special) {   
        break;
      }
      if (i < 3 && (i == 1 || buf1[0] < '3' || buf1[1] < '7')) { // max valid radix is 36
        buf1[i] = '\0';
        base = strtol(buf1, NULL, 10);
        i = 0;
        special = 'r';
      } else {
        free(buf1);
        lerror(state->toplevel, "Radix out of range");
      }
    } else if (c == '/') {
      if (special) {
        break;
      }
      special = c;
      buf1[i] = '\0';
      buf2 = buf1;
      buf1 = malloc(sz);
      i = 0;
    } else if (c == '.' || ((c == 'e' || c == 'E') && base == 10)) {
      if (special || (special == '.' && c == '.')) {
        break;
      }
      special = c == 'E' ? 'e' : c;;
      buf1[i++] = c;
      if (special == 'e') {
        c = ios_getc(state->input);
        if (c == '-' || c == '+' || (c >= '0' && c <= '9')) {
          buf1[i++] = c;
        } else {
          break;
        }
      }
    } else if ((c == 'N' || c == 'M') && base == 10) {
      // make sure the next char terminates the number
      c = ios_getc(state->input);
      if (c == IOS_EOF || c == iswhitespace(c) || ismacro(c)) {
        ios_ungetc(c, state->input);
      } else {
        break;
      }
    } else if (c >= '0' && ((base < 11 && (c < '0' + base)) || 
                            (base > 10 && (c <= '9' || 
                                           (c >= 'a' && c < 'a' + (base-10)) ||
                                           (c >= 'A' && c < 'A' + (base-10))
                                           )))) {
      buf1[i++] = c;
    } else {
      break;
    }
  }
  
  // note: should only get here when we have an invalid number
  free(buf1);
  if (buf2 != NULL) {
    free(buf2);
  }
  // TODO: to match clojure's error, we need the whole number
  // consider reading the whole number before processing it
  lerror(state->toplevel, "Invalid number");
  return 0;
}

// TODO: perhaps rename this to read_symbol, as that's what it really does
value_t read_token(readerstate_t *state, char tok) {
  int i = 0;
  // TODO: an optimization here would be to have all the buffers
  // for this and the string/number functions in the state so
  // we reuse the same part of memory and reduce the number of
  // mallocs we use
  size_t sz = 256;
  char *buf = malloc(sz);
  ios_ungetc(tok, state->input);
  while (1) {
    char c = nextchar(state->input, 0);
    if (c == IOS_EOF || iswhitespace(c) || (c != '#' && ismacro(c))) {
      ios_ungetc(c, state->input);
      break;
    } 
    buf[i++] = c;
    if (i >= sz-1) {
      // resize here to support 'infinite' length 'everything'
      // TODO: maybe choose a less greedy expansion here?
      // just remember, the less mallocs the better!
      char *tmp = realloc(buf, sz * 2);
      if (tmp == NULL) {
        lerror(state->toplevel, "out of memory");
      }
      sz *= 2;
      buf = tmp;
    }
  }
  buf[i] = '\0';
  value_t sym = symbol(state->namespace, buf);
  free(buf);
  return sym;
}
