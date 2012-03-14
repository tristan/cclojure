#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "numbers.h"
#include "regex.h"
#include "string_buffer.h"
#include "lisp_reader.h"
#include "unicode_utils.h"
#include <unicode/ustdio.h>
#include <unicode/uchar.h>

static regex_t *int_regex = NULL;
static regex_t *float_regex = NULL;
static regex_t *ratio_regex = NULL;

U_STRING_DECL(int_pat, "([-+]?)(?:(0)|([1-9][0-9]*)|0[xX]([0-9A-Fa-f]+)|0([0-7]+)|([1-9][0-9]?)[rR]([0-9A-Za-z]+)|0[0-9]+)", 99);
U_STRING_DECL(fl_pat, "([-+]?[0-9]+(\\.[0-9]*)?([eE][-+]?[0-9]+)?)(M)?", 47);
U_STRING_DECL(ra_pat, "([-+]?[0-9]+)/([0-9]+)", 23);

static int valid_macros[256];

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

  valid_macros['"'] = 1;
  valid_macros[';'] = 1;
  valid_macros['\''] = 1;
  valid_macros['@'] = 1;
  valid_macros['^'] = 1;
  valid_macros['`'] = 1;
  valid_macros['~'] = 1;
  valid_macros['('] = 1;
  valid_macros[')'] = 1;
  valid_macros['['] = 1;
  valid_macros[']'] = 1;
  valid_macros['{'] = 1;
  valid_macros['}'] = 1;
  valid_macros['\\'] = 1;
  valid_macros['%'] = 1;
  valid_macros['#'] = 1;
}

static void destroy() {
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
  return valid_macros[ch];
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

static Number *read_number(UFILE* in, UChar ch) {

  StringBuffer *sb = StringBuffer_new();
  StringBuffer_append_char(sb, ch);

  for (;;) {
    ch = u_fgetc(in);
    if (ch == U_EOF || iswhitespace(ch) || ismacro(ch)) {
      u_fseek(in, -1, SEEK_CUR);
      break;
    }
    StringBuffer_append_char(sb, ch);
  }

  UChar *str = StringBuffer_to_string(sb);
  Number *n = match_number(str);

  if (n == NULL) {
    u_printf("Invalid Number: %S\n", str);
  } else {
    UChar *sn = Number_to_str(n);
    u_printf("Got Number: %S\n", sn);
    free(sn);
  }

  free(str);
  StringBuffer_destroy(sb);
  return n;
}

static UChar read_unicode_char(UFILE *in, UChar ch, int32_t base, int32_t length, int extract) {
  return 0;
}

static UChar *read_string(UFILE *in) {
  StringBuffer *sb = StringBuffer_new();

  for (UChar ch = u_fgetc(in); ch != '"'; ch = u_fgetc(in)) {
    if (ch == -1) {
      StringBuffer_destroy(sb);
      puts("EOF while reading string");
      return NULL;
    }
    if (ch == '\\') { // escape
      ch = u_fgetc(in);
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
        ch = u_fgetc(in);
        if (u_isdigit(ch) || (ch >= 'a' && ch < 'g') || (ch >= 'A' && ch < 'G')) {
          ch = read_unicode_char(in, ch, 16, 4, 1);
        } else {
          StringBuffer_destroy(sb);
          u_printf("Invalid unicode escape: \\u%C\n", ch);
          return NULL;
        }
        break;
      default:
        if (u_isdigit(ch)) {
          ch = read_unicode_char(in, ch, 8, 3, 0);
          if (ch > 0377) {
            StringBuffer_destroy(sb);
            puts("Octal escape sequence must be in range [0, 377].");
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

  UChar *str = StringBuffer_to_string(sb);
  StringBuffer_destroy(sb);
  return str;
}

static UChar *read_token(UFILE *in, UChar ch) {
  return NULL;
}

static void *interpret_token(void *token) {
  return NULL;
}

static void *invoke_macro(UFILE *in, UChar ch) {
  if (ch == '"') {
    return read_string(in);
  }
  return NULL;
}

void *parse_lisp(UFILE *in) {
  UChar ch;
  init();

  for (;;) {
    while (iswhitespace(ch = u_fgetc(in))) ;

    if (ch == U_EOF) {
      puts("EOF");
      return NULL;
    }

    if (u_isdigit(ch)) {
      return read_number(in, ch);
    }

    if (ismacro(ch)) {
      // TODO; check op macro return type?
      return invoke_macro(in, ch);
    }

    if (ch == '+' || ch == '-') {
      UChar ch2 = u_fgetc(in);
      if (u_isdigit(ch2)) {
        u_fseek(in, -1, SEEK_CUR);
        return read_number(in, ch);
      }
      // TODO: clj does an unread here, i'm not sure why!
      // the unread here isn't covered by fseek, since if
      // read_number is called, ch2 will not be at pos SEEK_CUR-1
    }

    UChar *token = read_token(in, ch);
    interpret_token(token);
  }

  return NULL;
}

