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

  u_int32_t toktype; // the last read token's type
  value_t tokval;

  // setjmp variable
  jmp_buf toplevel;
} readerstate_t;

enum {
  TOK_NONE = 0,
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
  TOK_DISPATCH,
  TOK_NUM
};

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

typedef value_t(*macro_fn)(readerstate_t*, char tok);
static macro_fn macros[] = {
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

static void take(readerstate_t *state) {
  state->toktype = TOK_NONE;
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
  } else if (c == '+' || c == '-' || isdigit(c)) {
    toktype = TOK_NUM;
  }
  state->toktype = toktype;
  return state->toktype;
}

static value_t read(readerstate_t *state) {
  u_int32_t t = peek(state);
  take(state);
  value_t rval = macros[t](state, t);
  return rval;
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

  if (setjmp(rs.toplevel)) {
    //fprintf(stderr, "\nerror reading input\n");
    return 0;
  } else {
    return read(&rs);
  }
}

value_t handle_none(readerstate_t *state, u_int32_t tok) {
  return -1;
}

value_t handle_unexpected_token(readerstate_t *state, u_int32_t tok) {
  char c;
  switch (tok) {
  case TOK_CLOSE:
    c = ')';
    break;
  case TOK_CLOSEBRACKET:
    c = ']';
    break;
  case TOK_CLOSEBRACE:
    c = '}';
    break;
  default:
    c = '\0';
  }
  lerror(state->toplevel, "Unmatched delimiter: %c\n", c);
  return 0;
}

value_t read_list(readerstate_t *state, u_int32_t tok) {
  return -1;
}

value_t read_vector(readerstate_t *state, u_int32_t tok) {
  return -1;
}

value_t read_map(readerstate_t *state, u_int32_t tok) {
  return -1;
}

value_t read_comment(readerstate_t *state, u_int32_t tok) {
  return -1;
}

value_t read_string(readerstate_t *state, u_int32_t tok) {
  return -1;
}

value_t wrapped_read(readerstate_t *state, u_int32_t tok) {
  return -1;
}

value_t read_meta(readerstate_t *state, u_int32_t tok) {
  return -1;
}

value_t read_syntax_quote(readerstate_t *state, u_int32_t tok) {
  return -1;
}

value_t read_unquote(readerstate_t *state, u_int32_t tok) {
  return -1;
}

value_t read_char(readerstate_t *state, u_int32_t tok) {
  return -1;
}

value_t read_arg(readerstate_t *state, u_int32_t tok) {
  return -1;
}

value_t read_dispatch(readerstate_t *state, u_int32_t tok) {
  return -1;
}

value_t read_number(readerstate_t *state, u_int32_t tok) {
  return -1;
}
