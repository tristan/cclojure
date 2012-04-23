#include <sstream>
#include "clojure.h"
#include "utf8.h"

Character::Character(long c) : ch(c) {}

std::string Character::toString() const {
  std::stringstream ss;
  u8_wc_toutf8(ss, ch);
  return ss.str();
}

bool Character::operator==(const Object &o) const {
  try {
    const Character &c = dynamic_cast<const Character&>(o);
    return this->ch == c.ch;
  } catch (std::bad_cast &e) {
    // TODO: same as string::operator==
    return false;
  }
}
