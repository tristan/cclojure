#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unicode/ustdio.h>
#include <unicode/uchar.h>

#include "numbers.h"
#include "regex.h"
#include "string.h"
#include "string_buffer.h"
#include "lisp_reader.h"
#include "unicode_utils.h"
#include "RT.h"

static regex_t *int_regex = NULL;
static regex_t *float_regex = NULL;
static regex_t *ratio_regex = NULL;

U_STRING_DECL(int_pat, "([-+]?)(?:(0)|([1-9][0-9]*)|0[xX]([0-9A-Fa-f]+)|0([0-7]+)|([1-9][0-9]?)[rR]([0-9A-Za-z]+)|0[0-9]+)", 99);
U_STRING_DECL(fl_pat, "([-+]?[0-9]+(\\.[0-9]*)?([eE][-+]?[0-9]+)?)(M)?", 47);
U_STRING_DECL(ra_pat, "([-+]?[0-9]+)/([0-9]+)", 23);

typedef Object*(*macro_fn)(UFILE *in, UChar ch);
static macro_fn macros[256];

// private functions
static Object *read_string(UFILE *in, UChar ch);
static Object *read_comment(UFILE *in, UChar ch);
static Object *read_meta(UFILE *in, UChar ch);
static Object *read_list(UFILE *in, UChar ch);

static int initialised = 0;
static void init() {
  if (int_regex == NULL) {
    U_STRING_INIT(int_pat, "([-+]?)(?:(0)|([1-9][0-9]*)|0[xX]([0-9A-Fa-f]+)|0([0-7]+)|([1-9][0-9]?)[rR]([0-9A-Za-z]+)|0[0-9]+)", 99);
    int_regex = re_compile(int_pat);
  }

  if (float_regex == NULL) {
    U_STRING_INIT(fl_pat, "([-+]?[0-9]+(\\.[0-9]*)?([eE][-+]?[0-9]+)?)(M)?", 47);
    float_regex = re_compile(fl_pat);
  }

  if (ratio_regex == NULL) {
    U_STRING_INIT(ra_pat, "([-+]?[0-9]+)/([0-9]+)", 23);
    ratio_regex = re_compile(ra_pat);
  }

  macros['"'] = read_string;
  macros[';'] = read_comment;
  macros['\''] = 0;
  macros['@'] = 0;
  macros['^'] = read_meta;
  macros['`'] = 0;
  macros['~'] = 0;
  macros['('] = read_list;
  macros[')'] = 0;
  macros['['] = 0;
  macros[']'] = 0;
  macros['{'] = 0;
  macros['}'] = 0;
  macros['\\'] = 0;
  macros['%'] = 0;
  macros['#'] = 0;
}

void LispReader_shutdown() {
  regex_destroy(int_regex);
  regex_destroy(float_regex);
  regex_destroy(ratio_regex);
}

static int iswhitespace(UChar ch) {
  if (u_isblank(ch) || ch == ',') {
    return 1;
  } else {
    return 0;
  }
}

static int ismacro(UChar ch) {
  return macros[ch] != 0 ? 1 : 0;
}

static Number *match_number(const UChar* s) {

  regmatch_t *match = re_match(int_regex, s);
  if (match) {
    UChar *n = NULL;
    n = re_group(match, 2);
    if (n != NULL) {
      Number *num = Integer_init_int(0);
      regmatch_destroy(match);
      free(n);
      return num;
    }
    int negate = 0;
    n = re_group(match, 1);
    if (n != NULL) {
      if (n[0] == '-')
        negate = 1;
      free(n);
    }
    int radix = 10;
    if ((n = re_group(match, 3)) != NULL) {
      radix = 10;
    } else if ((n = re_group(match, 4)) != NULL) {
      radix = 16;
    } else if ((n = re_group(match, 5)) != NULL) {
      radix = 8;
    } else if ((n = re_group(match, 7)) != NULL) {
      UChar *tmp = re_group(match, 6);
      radix = u_utoi(tmp);
      free(tmp);
    }
    if (n == NULL) {
      return NULL;
    } else {
      Number *num = Integer_init_str(n, radix);
      if (negate) {
        Integer_negate(num);
      }
      free(n);
      regmatch_destroy(match);
      return num;
    }
  }

  match = re_match(float_regex, s);
  if (match) {
    UChar *n = NULL;
    Number *num = NULL;
    if ((n = re_group(match, 4)) != NULL) {
      num = Float_init_str(n);
      free(n);
    } else {
      num = Float_init_str(s);
    }
    regmatch_destroy(match);
    return num;
  }

  match = re_match(ratio_regex, s);
  if (match) {
    UChar *snum, *sden;
    snum = re_group(match, 1);
    sden = re_group(match, 2);
    Number *rat = Ratio_init_str(snum, sden);
    
    free(snum);
    free(sden);

    regmatch_destroy(match);
    return rat;
  }
  return NULL;
}

static Object *read_number(Reader* r, UChar ch) {

  StringBuffer *sb = StringBuffer_new();
  StringBuffer_append_char(sb, ch);

  for (;;) {
    ch = r->read(r);
    if (ch == U_EOF || iswhitespace(ch) || ismacro(ch)) {
      r->unread(r, ch);
      break;
    }
    StringBuffer_append_char(sb, ch);
  }

  UChar *str = StringBuffer_toString(sb);
  Number *n = match_number(str);

  if (n == NULL) {
    u_printf("Invalid Number: %S\n", str);
  }

  free(str);
  StringBuffer_destroy(sb);
  return Object_wrap_Number(n);
}

static UChar read_unicode_char(Reader *r, UChar initch, int32_t base, int32_t length, int exact) {
  int32_t uc = u_digit(initch, base);
  if (uc == -1) {
    u_printf("Invalid digit: %d\n", uc);
    return -1;
  }
  int i = 1;
  for (; i < length; ++i) {
    UChar ch = r->read(r);
    if (ch == -1 || iswhitespace(ch) || ismacro(ch)) {
      r->unread(r, ch);
      break;
    }
    int32_t d = u_digit(ch, base);
    if (d == -1) {
      u_printf("Invalid digit: %d\n", uc);
      return -1;
    }
    uc = uc * base + d;
  }
  if (i != length && exact) {
    u_printf("Invalid character length: %d, should be: %d\n", i, length);
    return -1;
  }
  return uc;
}

static Object *read_string(Reader *r, UChar dblq) {
  StringBuffer *sb = StringBuffer_new();

  for (UChar ch = r->read(r); ch != '"'; ch = r->read(r)) {
    if (ch == -1) {
      StringBuffer_destroy(sb);
      puts("EOF while reading string");
      return NULL;
    }
    if (ch == '\\') { // escape
      ch = r->read(r);
      if (ch == -1) {
        StringBuffer_destroy(sb);
        puts("EOF while reading string");
        return NULL;
      }
      switch (ch) {
      case 't':
        ch = '\t';
        break;
      case 'r':
        ch = '\r';
        break;
      case 'n':
        ch = '\n';
        break;
      case '\\':
        break;
      case '"':
        break;
      case 'b':
        ch = '\b';
        break;
      case 'f':
        ch = '\f';
        break;
      case 'u': // unicode
        ch = r->read(r);
        //if (u_isdigit(ch) || (ch >= 'a' && ch < 'g') || (ch >= 'A' && ch < 'G')) {
        if (u_digit(ch, 16) != -1) {
          ch = read_unicode_char(r, ch, 16, 4, 1);
          if (ch == -1) {
            StringBuffer_destroy(sb);
            return NULL;
          }
        } else {
          StringBuffer_destroy(sb);
          u_printf("Invalid unicode escape: \\u%C\n", ch);
          return NULL;
        }
        break;
      default:
        if (u_isdigit(ch)) {
          ch = read_unicode_char(r, ch, 8, 3, 0);
          if (ch > 0377) {
            StringBuffer_destroy(sb);
            puts("Octal escape sequence must be in range [0, 377].");
            return NULL;
          } else if (ch == -1) {
            StringBuffer_destroy(sb);
            return NULL;
          }
        } else {
          StringBuffer_destroy(sb);
          u_printf("Unsupported escape character: \\%C\n", ch);
          return NULL;
        }
      }
    }
    StringBuffer_append_char(sb, ch);
  }

  UChar *str = StringBuffer_toString(sb);
  StringBuffer_destroy(sb);
  String *s = String_new(str);
  free(str);
  return (Object*)s;
}

static Object *read_comment(Reader *r, UChar semicolon) {
  UChar ch;
  do {
    ch = r->read(r);
  } while (ch != U_EOF && ch != '\n' && ch != '\r');
  return r;
}

static Object *read_meta(Reader *r, UChar ch) {
  int line = -1;
  // TODO: change UFILE to a Reader of some kind
  Object *meta = parse_lisp(in, 1, NULL, 1);
  if (meta == NULL) {
    puts("Error getting meta");
    return NULL;
  }
  if (meta->type == SYMBOL_OBJ || meta->type == KEYWORD_OBJ || meta->type == STRING_OBJ) {
    //meta = Object_wrap_Map(RT_map(RT_TAG_KEY, meta));
    // TODO:
  } else if (meta->type != MAP_OBJ) {
    puts("Metadata must be Symbol,Keyword,String or Map");
    return NULL;
  }

  Object *o = parse_lisp(in, 1, NULL, 1);
  if (o->type == META_OBJ) {
    // TODO:
  } else {
    puts("Metadata can only be applied to IMetas");
    return NULL;
  }

  return NULL;
}

static Object *read_list(UFILE *in, UChar leftparen) {
  List *list = read_delimited_list(')', r, 1);
}

static UChar *read_token(UFILE *in, UChar ch) {
  return NULL;
}

static void *interpret_token(void *token) {
  return NULL;
}

static Object *invoke_macro(UFILE *in, UChar ch) {
  return macros[ch](in, ch);
}

Object *parse_lisp(Reader *r, int eof_is_error, Object *eof_value, int is_recursive) {
  if (!initialised) {
    init();
  }

  UChar ch;
  for (;;) {
    while (iswhitespace(ch = r->read(r)) ;

    if (ch == U_EOF) {
      if (eof_is_error) {
        puts("EOF");
        return NULL;
      } else {
        return eof_value;
      }
    }

    if (u_isdigit(ch)) {
      return (Object*)read_number(in, ch);
    }

    if (ismacro(ch)) {
      // TODO; check op macro return type?
      return invoke_macro(in, ch);
    }

    if (ch == '+' || ch == '-') {
      UChar ch2 = r->read(r);
      if (u_isdigit(ch2)) {
        r->unread(r, ch2);
        return read_number(in, ch);
      }
      // TODO: clj does an unread here, i'm not sure why!
      // the unread here isn't covered by fseek, since if
      // read_number is called, ch2 will not be at pos SEEK_CUR-1
    }

    String *token = read_token(in, ch);
    interpret_token(token);
  }

  return NULL;
}

