#ifndef _CLJ_SET_H
#define _CLJ_SET_H

class Set : public Object, public Meta {
public:
  Set();
  Set(const std::list<std::shared_ptr<Object> > &init);
  Set(std::shared_ptr<Map> meta, 
      const std::set<std::shared_ptr<Object>,compare_hashcodes> &set);

  std::string toString() const override;

  size_t count() const;

  bool instanceof(const std::type_info &info) const override;

  std::shared_ptr<Map> meta() const override;
  std::shared_ptr<Object> withMeta(std::shared_ptr<Map> meta) const override;
private:
  // TODO: java clojure's impl is more complex
  std::set<std::shared_ptr<Object>,compare_hashcodes> set;
  std::shared_ptr<Map> _meta;
};

#endif
