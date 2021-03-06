#ifndef _CLJ_LIST_H
#define _CLJ_LIST_H

#include <list>
#include <initializer_list>

class List : public Object, public Seq, public Meta {
public:
  List();
  // since the object here could be nil (i.e. nullptr) 
  List(std::shared_ptr<Object> first);
  //List(std::unique_ptr<Object> first);
  List(const std::list<std::shared_ptr<Object> > &init);
  List(std::shared_ptr<Object> first, std::shared_ptr<List> rest);
  List(std::shared_ptr<Map> meta, std::shared_ptr<Object> first, std::shared_ptr<List> rest);

  List(std::initializer_list<std::shared_ptr<Object> > init);

  std::string toString() const override;

  size_t count() const override;
  std::shared_ptr<Object> first() const override;
  std::shared_ptr<Seq> rest() const override;
  std::shared_ptr<Seq> cons(std::shared_ptr<Object> o) const override;
  
  bool instanceof(const std::type_info &info) const override;

  std::shared_ptr<Map> meta() const override;
  std::shared_ptr<Object> withMeta(std::shared_ptr<Map> meta) const override;
private:
  // shared ptr's should be ok here, as long as we don't
  // allow any calls which may mutate the list
  std::shared_ptr<Object> _first;
  std::shared_ptr<List> _rest;
  size_t _count;

  std::shared_ptr<Map> _meta;
};

#endif
