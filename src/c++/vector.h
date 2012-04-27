#ifndef _CLJ_VECTOR_H
#define _CLJ_VECTOR_H

#include <list>
#include <vector>

class Vector : public Object, public Seq, public Meta {
public:
  Vector();
  Vector(std::shared_ptr<Seq> items);
  Vector(const std::list<std::shared_ptr<Object> > &init);
  Vector(std::shared_ptr<Map> meta, const std::vector<std::shared_ptr<Object> > &vec);

  std::string toString() const override;

  size_t count() const override;
  std::shared_ptr<Object> first() const override;
  std::shared_ptr<Seq> rest() const override;
  std::shared_ptr<Seq> cons(std::shared_ptr<Object> o) const override;
  
  bool instanceof(const std::type_info &info) const override;

  std::shared_ptr<Map> meta() const override;
  std::shared_ptr<Object> withMeta(std::shared_ptr<Map> meta) const override;

private:
  // TODO: vectors are actually a lot more complex than this
  // but i'm going to worry about complex implementation
  // after i've got the basics going
  std::vector<std::shared_ptr<Object> > vec;  
  std::shared_ptr<Map> _meta;
};

#endif
