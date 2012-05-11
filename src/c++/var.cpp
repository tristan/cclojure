#include <thread>
#include "clojure.h"

/*
class Frame {
public:

  Frame() {};
public:
  std::shared_ptr<Frame> prev;
};

__thread Frame dvals;
//thread_local Frame dvals;
*/

class TBox : public Object {
public:
  TBox(std::thread::id t, std::shared_ptr<Object> val)
    : thread(t), val(val) {};
  
  std::thread::id thread;
  std::shared_ptr<Object> val;

  // TODO: these aren't needed, but they need to be implemented
  // so we can store the thread bindings in a Map
  std::string toString() const override {
    return "";
  };
  bool instanceof(const std::type_info &info) const override {
    return false;
  };

};

// TODO: cannot make non-trivial types thread-local, so, attempting a work-arround
// which uses a map with the thread id as the key.
//__thread std::stack<std::shared_ptr<Map>> __bindingstack;
std::map<std::thread::id,std::stack<std::shared_ptr<Map>>> __bindingstack;
int __rev = 0;

void Var::pushThreadBindings(std::shared_ptr<Map> bindings) {
  std::shared_ptr<Map> bmap = __bindingstack[std::this_thread::get_id()].top();
  for (auto it = bindings->begin(); 
        it != bindings->end();
        ++it) {
    auto e = *it;
    std::shared_ptr<Var> v = std::dynamic_pointer_cast<Var>(std::get<0>(e));
    if (!v->dynamic) {
      throw "Can't dynamically bind non-dynamic var: " + 
        v->ns->toString() + "/" + v->sym->toString();
    }
    //v->validate(v->getValidator(), std::get<1>(e));
    v->threadBound = true;
    std::thread::id tid = std::this_thread::get_id();
    bmap = bmap->assoc(v, std::make_shared<TBox>(tid, std::get<1>(e)));
  }
  __bindingstack[std::this_thread::get_id()].push(bmap);
}

Var::Var(std::shared_ptr<Namespace> ns, std::shared_ptr<Symbol> sym) 
  : ns(ns), sym(sym), threadBound(false), root(nullptr), dynamic(false) {}

Var::Var(std::shared_ptr<Namespace> ns, std::shared_ptr<Symbol> sym, std::shared_ptr<Object> root) 
  : ns(ns), sym(sym), threadBound(false), root(root), dynamic(false) {
  ++__rev;
}

Var::Var(std::shared_ptr<Namespace> ns, std::shared_ptr<Symbol> sym, bool dynamic) 
  : ns(ns), sym(sym), threadBound(false), root(nullptr), dynamic(dynamic) {}

Var::Var(std::shared_ptr<Namespace> ns, std::shared_ptr<Symbol> sym, std::shared_ptr<Object> root, bool dynamic) 
  : ns(ns), sym(sym), threadBound(false), root(root), dynamic(dynamic) {
  ++__rev;
}

std::shared_ptr<Var> Var::create() {
  return std::make_shared<Var>(nullptr, nullptr);
}

std::shared_ptr<Var> Var::create(std::shared_ptr<Object> root) {
  return std::make_shared<Var>(nullptr, nullptr, root);
}

std::shared_ptr<Var> Var::createDynamic() {
  std::shared_ptr<Var> var = Var::create();
  var->setDynamic();
  return var;
}

std::shared_ptr<Var> Var::createDynamic(std::shared_ptr<Object> root) {
  std::shared_ptr<Var> var = Var::create(root);
  var->setDynamic();
  return var;
}

std::string Var::toString() const {
  if (ns != nullptr) {
    return std::string("#'") + ns->toString() + "/" + sym->toString();
  }
  return std::string("#<Var: ") + (sym != nullptr ? sym->toString() : "--unnamed--") + ">";
}

bool Var::instanceof(const std::type_info &info) const {
  return (
          typeid(Var) == info
          );
}

void Var::setDynamic() {
  this->dynamic = true;
}

bool Var::isDynamic() {
  return this->dynamic;
}

std::shared_ptr<Object> Var::getThreadBinding() {
  if (threadBound) {
    std::shared_ptr<Map> bmap = __bindingstack[std::this_thread::get_id()].top();
    auto e = bmap->entryAt( shared_from_this() );
    if (std::get<0>(e) != nullptr) {
      return std::get<1>(e);
    }
  }
  return nullptr;
}

std::shared_ptr<Object> Var::deref() {
  std::shared_ptr<TBox> b = std::dynamic_pointer_cast<TBox>(this->getThreadBinding());
  if (b != nullptr) {
    return b->val;
  }
  return root;
}

std::shared_ptr<Object> Var::set(std::shared_ptr<Object> val) {
  // TODO: validate val
  std::shared_ptr<TBox> b = std::dynamic_pointer_cast<TBox>(this->getThreadBinding());
  if (b != nullptr) {
    if (std::this_thread::get_id() != b->thread) {
      throw std::string{ "Can't set!: " } + sym->toString() + "from non-binding thread";
    }
    b->val = val;
    return val;
  }
  throw std::string{"Can't change/establish root binding of: "} + sym->toString() + "with set";
                                                                  
}
