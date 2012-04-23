#include <iostream>
#include <string>

size_t u8_wc_toutf8(std::ostream &out, long ch) {
  if (ch < 0x80) {
    out.put((char)ch);
    return 1;
  }
  if (ch < 0x800) {
    out.put((ch>>6) | 0xC0);
    out.put((ch & 0x3F) | 0x80);
    return 2;
  }
  if (ch < 0x10000) {
    out.put((ch>>12) | 0xE0);
    out.put(((ch>>6) & 0x3F) | 0x80);
    out.put((ch & 0x3F) | 0x80);
    return 3;
  }
  if (ch < 0x110000) {
    out.put((ch>>18) | 0xF0);
    out.put(((ch>>12) & 0x3F) | 0x80);
    out.put(((ch>>6) & 0x3F) | 0x80);
    out.put((ch & 0x3F) | 0x80);
    return 4;
  }
  return 0;
}

std::string u8_wc_toutf8str(long ch) {
  if (ch < 0x80) {
    return std::string{(char)ch};
  }
  if (ch < 0x800) {
    return std::string {
      (char)((ch>>6) | 0xC0),
      (char)((ch & 0x3F) | 0x80)
    };
  }
  if (ch < 0x10000) {
    return std::string {
      (char)((ch>>12) | 0xE0),
      (char)(((ch>>6) & 0x3F) | 0x80),
      (char)((ch & 0x3F) | 0x80)
    };
  }
  if (ch < 0x110000) {
    return std::string {
      (char)((ch>>18) | 0xF0),
      (char)(((ch>>12) & 0x3F) | 0x80),
      (char)(((ch>>6) & 0x3F) | 0x80),
      (char)((ch & 0x3F) | 0x80)
    };
  }
  return "";
}
