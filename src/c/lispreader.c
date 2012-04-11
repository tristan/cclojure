#include <stdlib.h>
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

  // token reading buffer
  char *buf;
  int bufsize;

  // setjmp variable
  jmp_buf toplevel;
} readerstate_t;

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
  return (c > 0 && c < 128 && macros[c] != NULL) ? 1 : 0;
}

static void accumchar(readerstate_t *state, char c, int *pi)
{
  state->buf[(*pi)++] = c;
  if (*pi >= state->bufsize-1) {
    // resize here to support 'infinite' length 'everything'
    // TODO: maybe choose a less greedy expansion here?
    // just remember, the less mallocs the better!
    char *tmp = realloc(state->buf, state->bufsize * 2);
    if (tmp == NULL) {
      lerror(state->toplevel, "out of memory");
    }
    state->bufsize *= 2;
    state->buf = tmp;
  }
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

static value_t read(readerstate_t *state) {
  char c = nextchar(state->input, 1);
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
      return read_number(state, c);
    }
    if (ismacro(c)) {
      return macros[(int)c](state, c);
    }
    return read_token(state, c);
  }
}

value_t lispreader_read(ios_t *in, char eof_is_error, value_t eof_value, char is_recursive) {

  readerstate_t rs = {
    .input = in,
    .eof_value = eof_value,
    .recursive = is_recursive,
    .error_on_eof = eof_is_error
  };
  rs.bufsize = 256;
  rs.buf = malloc(rs.bufsize);

  if (setjmp(rs.toplevel)) {
    // an error occured, clean up
    free(rs.buf);
    return -1;
  } else {
    if (macros[0] != NULL) {
      lerror(rs.toplevel, "macros[0] is not null!");
    }
    return read(&rs);
  }
}

value_t handle_unexpected_token(readerstate_t *state, char tok) {
  lerror(state->toplevel, "Unmatched delimiter: %c", tok);
  return 0;
}

value_t read_list(readerstate_t *state, char tok) {
  lerror(state->toplevel, "not yet implemented");
  return 0;
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
  free(buf);
  return 0;
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
  lerror(state->toplevel, "not yet implemented");
  return 0;
}

value_t read_token(readerstate_t *state, char tok) {
  int i = 0;
  ios_ungetc(tok, state->input);
  while (1) {
    char c = nextchar(state->input, 0);
    if (c == IOS_EOF || iswhitespace(c) || (c != '#' && ismacro(c))) {
      ios_ungetc(c, state->input);
      break;
    } 
    accumchar(state, c, &i);
  }
  state->buf[i++] = '\0';
  // TODO: make into symbol
  return 0;
}
