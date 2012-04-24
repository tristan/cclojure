#ifndef _CLJ_SET_H
#define _CLJ_SET_H

class Set : public Object {
public:
  Set();
  Set(const std::list<std::shared_ptr<Object> > &init);

  std::string toString() const override;

  size_t count() const;
private:
  // TODO: java clojure's impl is more complex
  std::set<std::shared_ptr<Object>,compare_object_in_shared_ptr> set;
};

#endif
