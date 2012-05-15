#ifndef _CLJ_RUNTIME_H
#define _CLJ_RUNTIME_H

#include <iostream>

namespace runtime {

  void print(clojure::object x, std::ostream &out);

}

#endif
