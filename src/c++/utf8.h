#ifndef _UTF8_H
#define _UTF8_H

size_t u8_wc_toutf8(std::ostream &out, long ch);
std::string u8_wc_toutf8str(long ch);

#endif
