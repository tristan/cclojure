#include "clojure.h"
#include "utf8.h"

Character::Character(long c) : ch(c) {}

std::string Character::toString() const {
  return u8_wc_toutf8str(ch);
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

int Character::compareTo(const Comparable &o) const {
  if (typeid(o) != typeid(Character)) {
    throw std::string(typeid(o).name()) + " cannot be cast to Character";
  }
  const Character &c = dynamic_cast<const Character &>(o);
  if (this->ch > c.ch) {
    return 1;
  }
  if (this->ch < c.ch) {
    return -1;
  }
  return 0;
}

bool Character::instanceof(const std::type_info &info) const {
  return (
          typeid(Character) == info ||
          typeid(Comparable) == info
          );
}
