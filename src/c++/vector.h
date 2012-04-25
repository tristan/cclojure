#ifndef _CLJ_VECTOR_H
#define _CLJ_VECTOR_H

#include <list>
#include <vector>

class Vector : public Object, public Seq {
public:
  Vector();
  Vector(std::shared_ptr<Seq> items);
  Vector(const std::list<std::shared_ptr<Object> > &init);

  std::string toString() const override;

  size_t count() const override;
  std::shared_ptr<Object> first() const override;
  std::shared_ptr<Seq> rest() const override;
  std::shared_ptr<Seq> cons(std::shared_ptr<Object> o) const override;
  
  bool instanceof(const std::type_info &info) const override;
private:
  // TODO: vectors are actually a lot more complex than this
  // but i'm going to worry about complex implementation
  // after i've got the basics going
  std::vector<std::shared_ptr<Object> > vec;
};

#endif
