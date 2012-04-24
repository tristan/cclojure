#ifndef _CLJ_MAP_H
#define _CLJ_MAP_H

#include <list>
#include <map>
#include <set>

class Map : public Object {
public:
  Map();
  Map(const std::list<std::shared_ptr<Object> > &init);

  std::string toString() const override;

  size_t count() const;
private:
  // TODO: java clojure's impl is more complex
  std::map<std::shared_ptr<Object>,std::shared_ptr<Object>,compare_object_in_shared_ptr> map;
};

#endif