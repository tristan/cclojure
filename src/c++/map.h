#ifndef _CLJ_MAP_H
#define _CLJ_MAP_H

#include <list>
#include <map>
#include <set>
//#include <initializer_list>

class Map : public Object, public Meta, public Iterable {
public:
  Map();
  Map(const std::list<std::shared_ptr<Object> > &init);
  //Map(std::initializer_list<std::shared_ptr<Object> > init);
  Map(std::shared_ptr<Map> meta, 
      const std::map<std::shared_ptr<Object>,std::shared_ptr<Object>,compare_hashcodes> &map);

  std::string toString() const override;
  size_t count() const;

  // TODO: all these probably need to override something!
  std::shared_ptr<Map> assoc(std::shared_ptr<Object>,std::shared_ptr<Object>) const;
  std::pair<std::shared_ptr<Object>,std::shared_ptr<Object> > entryAt(std::shared_ptr<Object> key);
  std::shared_ptr<Object> valAt(std::shared_ptr<Object> key);

  bool instanceof(const std::type_info &info) const override;

  std::shared_ptr<Map> meta() const override;
  std::shared_ptr<Object> withMeta(std::shared_ptr<Map> meta) const override;

  class iterator {
  public:
    iterator(std::map<std::shared_ptr<Object>,std::shared_ptr<Object>,compare_hashcodes>::const_iterator it);
    std::pair<const std::shared_ptr<Object>,std::shared_ptr<Object> > operator*();
    iterator& operator++();
    bool operator==(const iterator& rhs);
    bool operator!=(const iterator& rhs);
    
  private:
    std::map<std::shared_ptr<Object>,std::shared_ptr<Object>,compare_hashcodes>::const_iterator it;
  };

  iterator begin() const;
  iterator end() const;
private:
  // TODO: java clojure's impl is more complex
  std::map<std::shared_ptr<Object>,std::shared_ptr<Object>,compare_hashcodes> map;
  std::shared_ptr<Map> _meta;
};

#endif
