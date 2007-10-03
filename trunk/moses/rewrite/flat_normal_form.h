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

#ifndef FLAT_NORMAL_FORM_H
#define FLAT_NORMAL_FORM_H

#include "SubtreeOrder.h"
#include <ext/hash_map>
#include <set>
#include <list>
#include "math_util.h"
#include <boost/bind.hpp>
#include <numeric>
#include <map>
#include "id.h"

namespace rewrite {
  using namespace trees;
  using namespace id;
  typedef std::set<int,AdjacentNegativesComp> Clause;
  typedef std::list<Clause> NF;

  //does c contain p and !p?
  bool tautology(const Clause& c);
  //is c1 a subset of (or equal to) c2?
  bool subset_eq(const Clause& c1,const Clause& c2);
  bool subset(const Clause& c1,const Clause& c2);
  int number_of_literals(const NF& f);

  //(p X)(!p Y) implies (X Y); this function finds all such matches
  template<typename Out>
  void implications(const Clause& c1,const Clause& c2,Out out) {
    for (Clause::const_iterator it1=c1.begin();it1!=c1.end();++it1) {
      for (Clause::const_iterator it2=c2.begin();it2!=c2.end();++it2) {
	if (*it1==-*it2) {
	  Clause c;
	  std::merge(c1.begin(),c1.end(),c2.begin(),c2.end(),
		     inserter(c,c.begin()));
	  c.erase(*it1);
	  c.erase(*it2);
	  if (!tautology(c))
	    (*out++)=c;
	}
      }
    }
  }

  template<typename BinaryPred>
  void pairwise_erase_if(NF& f,const BinaryPred& p) {
    NF::iterator c1=f.begin();
    while (c1!=f.end()) {
      NF::iterator next1=c1,c2=f.begin();
      ++next1;
      while (c2!=c1) {
	NF::iterator next2=c2;
	++next2;
	if (c1!=c2) {
	  if (p(*c1,*c2)) {
	    f.erase(c2);
	  } else if (p(*c2,*c1)) {
	    f.erase(c1);
	    break;
	  }
	}
	c2=next2;
      }
      c1=next1;
    }
  }
  /**
  template<typename T>
  nf_to_tree(const NF& nf,tree<T> tr) {
    tr.clear();
    tr.insert(tr.begin(),T());
    for (NF::const_iterator clause=nf.begin();clause!=nf.end();++clause) {
      typename tree<T>::sibling_iterator it=tr.insert_after(tr.begin(),T());
      for_each(
      tr.append_child(it,
	 it!=params.proto.end();++it) {
    
  **/
  /**
  template<typename typename Op>
  void pairwise_apply(NF& f,const Op& op) {
    NF::iterator first=f.begin(),last=f.end();
    for (NF::iterator c1=first;c1!=last;++c1)
      for (c2=first;c2!=last;++c2)
	if (c1!=c2)
	  op(c1,c2);
  }
  **/

  template<typename T>
  class NFMapper {
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

    void extractConjunction(const Clause&,tree<T>&,sib_it) const;
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
	sib_it clause=t.append_child(sib,create_or<T>());
	for (Clause::const_iterator item=c->begin();item!=c->end();++item) {
	  create(t,clause,*item);
	}
      }
    if (sib.number_of_children()==1) {
      *sib=*sib.begin();
      t.erase(t.flatten(sib.begin()));
    } else {
      *sib=(sib.number_of_children()==0) ? create_false<T>() : create_and<T>();
    }
  }

  template<typename T>
  template<typename It>
  void NFMapper<T>::extractDNF(It from,It to,tree<T>& t,sib_it sib) const {
    t.erase_children(sib);
    for (NF::const_iterator c=from;c!=to;++c)
      if (c->empty()) {
	t.erase_children(sib);
	*sib=create_true<T>();
	return;
      } else if (c->size()==1) {
	if (*c->begin()!=0)
	  create(t,sib,*c->begin());
      } else {
	sib_it clause=t.append_child(sib,create_and<T>());
	for (Clause::const_iterator item=c->begin();item!=c->end();++item) {
	  create(t,clause,*item);
	}
      }
    if (sib.number_of_children()==1) {
      *sib=*sib.begin();
      t.erase(t.flatten(sib.begin()));
    } else {
      *sib=(sib.number_of_children()==0) ? create_false<T>() : create_or<T>();
    }
  }

  template<typename T>
  void NFMapper<T>::extractConjunction(const Clause& c,
				       tree<T>& t,sib_it sib) const {
    if (c.size()==1) {
      if (*c.begin()!=0) {
	if (*c.begin()<0) {
	  *sib=create_not<T>();
	  t.append_child(typename tree<T>::pre_order_iterator(sib),
			 _int2item.find(-*c.begin())->second.begin());
	} else {
	  typename tree<T>::pre_order_iterator tmp=
	    _int2item.find(*c.begin())->second.begin();
	  *sib=*tmp;
	  t.append_children(sib,tmp.begin(),tmp.end());
	}
      }
    } else {
      *sib=create_and<T>();
      for (Clause::const_iterator item=c.begin();item!=c.end();++item) {
	create(t,sib,*item);
      }
    }
  }

  template<typename T>
  int NFMapper<T>::addItem(sib_it item) {
    if (is_not(*item))
      return -addItem(item.begin());

    /** important: we assume no constants because this messes up
        independence assumptions
    if (is_const(*item))
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
		       (t.append_child(at,create_not<T>())),
		       _int2item.find(-idx)->second.begin());
      else
	t.append_child(typename tree<T>::pre_order_iterator(at),
		       _int2item.find(idx)->second.begin());
  }

} //~namespace rewrite

std::ostream& operator<<
  (std::ostream& out,const std::set<int,AdjacentNegativesComp>& c);
std::ostream& operator<<
  (std::ostream& out,const std::list<std::set<int,AdjacentNegativesComp> >& d);

#endif
