/****
   Copyright 2005-2007, Moshe Looks

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
****/

#ifndef TREE_UTIL_H
#define TREE_UTIL_H

#include <boost/iterator/indirect_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <numeric>

namespace trees {
  using namespace std;
  using namespace boost;
  
  template<typename iter>
  int number_of_leaves(iter it) {
    return it.begin()==it.end() ? (id::is_argument(*it) ? 1 : 0) :
      accumulate(make_transform_iterator(make_counting_iterator(it.begin()),
					 number_of_leaves<iter>),
		 make_transform_iterator(make_counting_iterator(it.end()),
					 number_of_leaves<iter>),0);
  }
  template<typename iter>
  int complexity(iter it) { return number_of_leaves(it)-1; }
  template<typename T>
  int complexity(const tree<T>& tr) { return complexity(tr.begin()); }

#if 0
  template<typename T>
  class TreeMapping : public MetaData<T> {
  public:
    typedef typename tree<T>::sibling_iterator sib_it;
    typedef std::map<sib_it,int,SubtreeOrder<T> > Item2Int;
    typedef __gnu_cxx::hash_map<int,tree<T> > Int2Item;

    
    NF addCNF(sib_it,sib_it);
    NF addDNF(sib_it,sib_it);
    NF addLiteral(sib_it lit);

    template<typename It>
    void extractCNF(It,It,tree<T>&,sib_it) const;
    template<typename It>
    void extractDNF(It,It,tree<T>&,sib_it) const;
  private:
    Item2Int _item2int;
    Int2Item _int2item;
    
    int addItem(sib_it);
    void create(tree<T>&,sib_it,int) const;
  };

  template<typename T>
  NF NFMapper<T>::addCNF(sib_it from,sib_it to) {
    NF res(distance(from,to));
    for (NF::iterator out=res.begin();out!=res.end();++from,++out)
      if (is_or(*from))
	for (sib_it item=from.begin();item!=from.end();++item)
	  out->insert(addItem(item));
      else
	out->insert(addItem(from));
    return res;
  }
  template<typename T>
  NF NFMapper<T>::addDNF(sib_it from,sib_it to) {
    NF res(distance(from,to));
    for (NF::iterator out=res.begin();out!=res.end();++from,++out)
      if (is_and(*from))
	for (sib_it item=from.begin();item!=from.end();++item)
	  out->insert(addItem(item));
      else 
	out->insert(addItem(from));
    return res;
  }
  template<typename T>
  NF NFMapper<T>::addLiteral(sib_it lit) { 
    Clause c;
    c.insert(addItem(lit));
    return NF(1,c);
  }

  template<typename T>
  template<typename It>
  void NFMapper<T>::extractCNF(It from,It to,tree<T>& t,sib_it sib) const {
    t.erase_children(sib);
    for (NF::const_iterator c=from;c!=to;++c)
      if (c->size()==1) {
	if (*c->begin()!=0)
	  create(t,sib,*c->begin());
      } else if (!c->empty()) {
	sib_it clause=t.append_child(sib,create_or());
	for (Clause::const_iterator item=c->begin();item!=c->end();++item) {
	  create(t,clause,*item);
	}
      }
    if (sib.number_of_children()==1)
      t.erase(t.flatten(sib));
    else
      *sib=(sib.number_of_children()==0) ? create_false() : create_and();
  }

  template<typename T>
  template<typename It>
  void NFMapper<T>::extractDNF(It from,It to,tree<T>& t,sib_it sib) const {
    t.erase_children(sib);
    for (NF::const_iterator c=from;c!=to;++c)
      if (c->empty()) {
	t.erase_children(sib);
	*sib=create_true();
	return;
      } else if (c->size()==1) {
	if (*c->begin()!=0)
	  create(t,sib,*c->begin());
      } else {
	sib_it clause=t.append_child(sib,create_and());
	for (Clause::const_iterator item=c->begin();item!=c->end();++item) {
	  create(t,clause,*item);
	}
      }
    if (sib.number_of_children()==1)
      t.erase(t.flatten(sib));
    else
      *sib=(sib.number_of_children()==0) ? create_false() : create_or();
  }

  template<typename T>
  int NFMapper<T>::addItem(sib_it item) {
    if (is_not(*item))
      return -addItem(item.begin());

    /** important: we assume no constants because this messes up
        independence assumptions
    if (is_constant(*item))
      return _int2item.insert(make_pair(_int2item.size()+1,
					item)).first->first;
    **/

    typename Item2Int::const_iterator item_loc=_item2int.find(item);
    return (item_loc==_item2int.end()) ?
      _item2int.insert
      (make_pair(item,
		 _int2item.insert
		 (make_pair(_int2item.size()+1,
			    item)).first->first)).first->second
      :
      item_loc->second;
  }

  template<typename T>
  void NFMapper<T>::create(tree<T>& t,sib_it at,int idx) const {
      if (idx<0)
	t.append_child(typename tree<T>::pre_order_iterator
		       (t.append_child(at,create_not())),
		       _int2item.find(-idx)->second.begin());
      else
	t.append_child(typename tree<T>::pre_order_iterator(at),
		       _int2item.find(idx)->second.begin());
  }
#endif
} //~namespace trees

#endif
