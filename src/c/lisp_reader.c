#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unicode/ustdio.h>
#include <unicode/uchar.h>

#include "empty_seq.h"
#include "numbers.h"
#include "regex.h"
#include "string.h"
#include "string_buffer.h"
#include "lisp_reader.h"
#include "line_number_reader.h"
#include "unicode_utils.h"
#include "arraylist.h"

static Pattern *int_regex = NULL;
static Pattern *float_regex = NULL;
static Pattern *ratio_regex = NULL;

const char int_patt[] = 
  "([-+]?)(?:(0)|([1-9][0-9]*)|0[xX]([0-9A-Fa-f]+)|0([0-7]+)|([1-9][0-9]?)[rR]([0-9A-Za-z]+)|0[0-9]+)";
const char *float_patt = "([-+]?[0-9]+(\\.[0-9]*)?([eE][-+]?[0-9]+)?)(M)?";
const char *ratio_patt = "([-+]?[0-9]+)/([0-9]+)";

typedef Object*(*macro_fn)(Reader *r, UChar ch);
static macro_fn macros[256];

// private functions
static Object *read_string(Reader *r, UChar ch);
static Object *read_comment(Reader *r, UChar ch);
static Object *read_meta(Reader *r, UChar ch);
static Object *read_list(Reader *r, UChar ch);

static int initialised = 0;
static void init() {
  if (int_regex == NULL) {
    int_regex = Pattern_compile_a(int_patt, 0);
  }

  if (float_regex == NULL) {
    float_regex = Pattern_compile_a(float_patt, 0);
  }

  if (ratio_regex == NULL) {
    ratio_regex = Pattern_compile_a(ratio_patt, 0);
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
  drop_ref(int_regex);
  drop_ref(float_regex);
  drop_ref(ratio_regex);
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
static Object *invoke_macro(Reader *r, UChar ch) {
  return macros[ch](r, ch);
}

static Number *match_number(const UChar* s) {

  Matcher *match = int_regex->matcher_u(int_regex, s);
  if (match->matches(match)) {
    String *n = NULL;
    n = match->group(match, 2);
    if (n != NULL) {
      Number *num = (Number*)Integer_new(0);
      drop_ref(match);
      drop_ref(n);
      return num;
    }
    int negate = 0;
    n = match->group(match, 1);
    if (n != NULL) {
      if (n->charAt(n, 0) == '-')
        negate = 1;
      drop_ref(n);
    }
    int radix = 10;
    if ((n = match->group(match, 3)) != NULL) {
      radix = 10;
    } else if ((n = match->group(match, 4)) != NULL) {
      radix = 16;
    } else if ((n = match->group(match, 5)) != NULL) {
      radix = 8;
    } else if ((n = match->group(match, 7)) != NULL) {
      String *ts = match->group(match, 6);
      UChar *tmp = ts->toString(ts);
      radix = u_utoi(tmp);
      free(tmp);
      drop_ref(ts);
    }
    if (n == NULL) {
      return NULL;
    } else {
      Number *num = (Number*)Integer_valueOf_s(n, radix);
      if (negate) {
        num->negate(num);
      }
      drop_ref(n);
      drop_ref(match);
      return num;
    }
  }

  match = float_regex->matcher_u(float_regex, s);
  if (match->matches(match)) {
    String *n = NULL;
    Number *num = NULL;
    if ((n = match->group(match, 4)) != NULL) {
      UChar *t = n->toString(n);
      num = (Number*)Decimal_valueOf(t);
      free(t);
      drop_ref(n);
    } else {
      num = (Number*)Decimal_valueOf(s);
    }
    drop_ref(match);
    return num;
  }

  match = ratio_regex->matcher_u(ratio_regex, s);
  if (match->matches(match)) {
    String *snum, *sden;
    snum = match->group(match, 1);
    sden = match->group(match, 2);
    Number *rat = (Number*)Ratio_new_s(snum, sden);
    
    drop_ref(snum);
    drop_ref(sden);

    drop_ref(match);
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
  return (Object*)n;
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
  String *s = String_new_u(str);
  free(str);
  return (Object*)s;
}

static Object *read_comment(Reader *r, UChar semicolon) {
  UChar ch;
  do {
    ch = r->read(r);
  } while (ch != U_EOF && ch != '\n' && ch != '\r');
  return (Object*)r;
}

static Object *read_meta(Reader *r, UChar ch) {
  /*
  int line = -1;
  Object *meta = parse_lisp(r, 1, NULL, 1);
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

  Object *o = parse_lisp(r, 1, NULL, 1);
  if (o->type == META_OBJ) {
    // TODO:
  } else {
    puts("Metadata can only be applied to IMetas");
    return NULL;
  }
  */
  return NULL;
}

List *read_delimited_list(UChar delim, Reader *r, int is_recursive) {
  List *a = (List*)ArrayList_new(32);
  UChar ch;
  for (;;) {
    while (iswhitespace(ch = r->read(r))) ;

    if (ch == U_EOF) {
      puts("EOF while reading list");
      return NULL;
    }

    if (ch == delim) {
      break;
    }

    if (ismacro(ch)) {
      Object *o = invoke_macro(r, ch);
      if (o != NULL) {
        a->add(a, o);
      }
    } else {
      r->unread(r, ch);
      Object *o = parse_lisp(r, 1, NULL, is_recursive);
      if (o != (Object*)r) {
        a->add(a, o);
      }
    }
  }

  return a;
}

static Object *read_list(Reader *r, UChar leftparen) {
  int line = -1;
  if (r->instanceOf(r, LINENUMBERREADER_CLASS)) {
    line = ((LineNumberReader*)r)->getLineNumber(r);
  }
  List *list = read_delimited_list(')', r, 1);
  if (list->isEmpty(list)) return (Object*)EmptySeq_new();
  if (line != -1) {
    return (Object*)list; // WITH META
  } else {
    return (Object*)list;
  }
}

static UChar *read_token(Reader *r, UChar ch) {
  return NULL;
}

static void *interpret_token(void *token) {
  return NULL;
}

Object *parse_lisp(Reader *r, int eof_is_error, Object *eof_value, int is_recursive) {
  if (!initialised) {
    init();
  }

  UChar ch;
  for (;;) {
    while (iswhitespace(ch = r->read(r))) ;
    if (ch == U_EOF) {
      if (eof_is_error) {
        puts("EOF in parse_lisp");
        return NULL;
      } else {
        return eof_value;
      }
    }

    if (u_isdigit(ch)) {
      return (Object*)read_number(r, ch);
    }

    if (ismacro(ch)) {
      // TODO; check op macro return type?
      return invoke_macro(r, ch);
    }

    if (ch == '+' || ch == '-') {
      UChar ch2 = r->read(r);
      if (u_isdigit(ch2)) {
        r->unread(r, ch2);
        return read_number(r, ch);
      }
      // TODO: clj does an unread here, i'm not sure why!
      // the unread here isn't covered by fseek, since if
      // read_number is called, ch2 will not be at pos SEEK_CUR-1
    }

    UChar *token = read_token(r, ch);
    interpret_token(token);
  }

  return NULL;
}

