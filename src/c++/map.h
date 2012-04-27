#ifndef _CLJ_MAP_H
#define _CLJ_MAP_H

#include <list>
#include <map>
#include <set>
//#include <initializer_list>

class Map : public Object, public Meta {
public:
  Map();
  Map(const std::list<std::shared_ptr<Object> > &init);
  //Map(std::initializer_list<std::shared_ptr<Object> > init);
  Map(std::shared_ptr<Map> meta, 
      const std::map<std::shared_ptr<Object>,std::shared_ptr<Object>,compare_hashcodes> &map);

  std::string toString() const override;
  size_t count() const;

  bool instanceof(const std::type_info &info) const override;

  std::shared_ptr<Map> meta() const override;
  std::shared_ptr<Object> withMeta(std::shared_ptr<Map> meta) const override;
private:
  // TODO: java clojure's impl is more complex
  std::map<std::shared_ptr<Object>,std::shared_ptr<Object>,compare_hashcodes> map;
  std::shared_ptr<Map> _meta;
};

#endif
