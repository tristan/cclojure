#include <stdio.h>
#include <ctype.h>
#include "dtypes.h"
#include "utils.h"
#include "utf8.h"
#include "timefuncs.h"
#include "ios.h"
#include "clojure.h"
#include "lispreader.h"

typedef struct {
  ios_t *input; // the input stream
  value_t eof_value; // the value of an eof
  char error_on_eof; // should an error be thrown if eof is reached?
  char recursive; // 

  u_int32_t toktype; // the last read token's type
  value_t tokval;
} readerstate_t;

enum {
  TOK_NONE,
  TOK_OPEN,
  TOK_CLOSE,
  TOK_OPENBRACKET,
  TOK_CLOSEBRACKET,
  TOK_OPENBRACE,
  TOK_CLOSEBRACE,
  TOK_COMMENT,
  TOK_STRING,
  TOK_QUOTE,
  TOK_DEREF,
  TOK_META,
  TOK_SYNTAXQUOTE,
  TOK_UNQUOTE,
  TOK_CHAR,
  TOK_ARG,
  TOK_DISPATCH
};

static char nextchar(ios_t *f) {
  int ch;
  char c;
  
  do {
    if (f->bpos < f->size) {
      ch = f->buf[f->bpos++];
    } else {
      ch = ios_getc(f);
      if (ch == IOS_EOF) {
        return 0;
      }
    }
    c = (char)ch;
  } while (c == ' ' || c == ',' || isspace(c));
  return c;
}

static u_int32_t peek(readerstate_t *state) {
  value_t toktype = state->toktype;
  if (toktype != TOK_NONE) {
    return toktype;
  }
  char c = nextchar(state->input);
  if (ios_eof(state->input)) {
    return TOK_NONE;
  }
  if (c == '(') {
    toktype = TOK_OPEN;
  } else if (c == ')') {
    toktype = TOK_CLOSE;
  } else if (c == '[') {
    toktype = TOK_OPENBRACKET;
  } else if (c == ']') {
    toktype = TOK_CLOSEBRACKET;
  } else if (c == '{') {
    toktype = TOK_OPENBRACE;
  } else if (c == '}') {
    toktype = TOK_CLOSEBRACE;
  } else if (c == ';') {
    toktype = TOK_COMMENT;
  } else if (c == '"') {
    toktype = TOK_STRING;
  } else if (c == '\'') {
    toktype = TOK_QUOTE;
  } else if (c == '@') {
    toktype = TOK_DEREF;
  } else if (c == '^') {
    toktype = TOK_META;
  } else if (c == '`') {
    toktype = TOK_SYNTAXQUOTE;
  } else if (c == '~') {
    toktype = TOK_UNQUOTE;
  } else if (c == '\\') {
    toktype = TOK_CHAR;
  } else if (c == '%') {
    toktype = TOK_ARG;
  } else if (c == '#') {
    toktype = TOK_DISPATCH;
  }
  state->toktype = toktype;
  return state->toktype;
}

static value_t read(readerstate_t *state) {
  return state->eof_value;
}

value_t lispreader_read(ios_t *in, char eof_is_error, value_t eof_value, char is_recursive) {
  
  readerstate_t rs = {
    .input = in,
    .eof_value = eof_value,
    .recursive = is_recursive,
    .error_on_eof = eof_is_error,
    .toktype = TOK_NONE,
    .tokval = 0
  };

  return read(&rs);
}
