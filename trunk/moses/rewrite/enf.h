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

#ifndef ENF_H
#define ENF_H

#include <set>
#include <utility>
#include <iterator>
#include <algorithm>
#include <functional>
#include <boost/bind.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/functional/hash.hpp>
#include <iostream>

#include "tree.h"
#include "logical_rewrite.h"
#include "func_util.h"
#include "set_util.h"

namespace rewrite {
  namespace enf {
    typedef int Constraint;
    typedef std::set<Constraint> Set;
    typedef std::pair<bool,Set> Node;
    typedef tree<Node> Tree;
    typedef Tree::pre_order_iterator pre_it;
    typedef Tree::sibling_iterator sib_it;

    bool consistent(const Set& s);

    enum Result { Delete,Disconnect,Keep };

    class reduce_to_elegance {
    public:
      reduce_to_elegance(Tree& tr) : _tr(tr) { 
	if (!tr.empty() && reduce(tr.begin(),Set(),Set())==Delete)
	  tr.clear(); //contradiction
      }
    private:
      Tree& _tr;

      Result reduce(pre_it current,const Set& dominant,const Set& command);

      bool and_cut(sib_it);
      void or_cut(sib_it);
    };
  } //~namespace enf
} //~namespace rewrite

namespace id {
  template<> 
  inline bool is_and(const rewrite::enf::Node& x) { return x.first; }
  template<>
  inline bool is_or(const rewrite::enf::Node& x) { return !x.first; }

  template<> inline rewrite::enf::Node create_and() { 
    return make_pair(true,rewrite::enf::Set()); 
  }
  template<> inline rewrite::enf::Node create_or() { 
    return make_pair(false,rewrite::enf::Set()); 
  }

  //calls that are only valid for enf::Node, hence no template<>
  inline rewrite::enf::Node create_and(rewrite::enf::Constraint literal) {
    rewrite::enf::Set tmp;
    tmp.insert(literal);
    return make_pair(true,tmp);
  }
  inline rewrite::enf::Set& guard_set(rewrite::enf::Node& x) { 
    return x.second; 
  }

  
} //~namespace id;

namespace rewrite {
  namespace enf {
    template<typename T,typename AndT,typename OrT>
    class ConstraintCalculus {
    public:
      typedef typename tree<T>::pre_order_iterator tpre_it;
      typedef typename tree<T>::sibling_iterator tsib_it;
      typedef std::map<Constraint,tpre_it> C2T;
      typedef std::map<tpre_it,Constraint,SubtreeOrder<T> > T2C;

      ConstraintCalculus(const AndT& a,const OrT& o)
	: is_and(a),is_or(o),_ctr(create_and<Node>()) { }
      
      template<typename It>
      void to_constraint_tree(tree<T>& tr,It src,Tree& ctree) const {
	tree<T> tmp;
	do {
	  tmp=tr;
	  reduce_nots(tr,src);
	  eval_logical_identities(tr,src);
	  level(tr,src);
	} while (tmp!=tr);
	
	ctree.clear();
	if (is_false(*src))
	  return;
	ctree.set_head(create_and<Node>());
	if (is_true(*src))
	  return;

	//cout << "creating from " << tr << endl;

	if (is_or(*src))
	  rec_make(src,_ctr.append_child(_ctr.begin(),create_or<Node>()));
	else
	  rec_make(src,_ctr.begin()); //an and or a literal
	std::swap(ctree,_ctr);

	//cout << "created ctree " << ctree << endl;
      }
      template<typename It>
      void from_constraint_tree(const Tree& ctree,tree<T>& tr,It dst) const {
	tr.erase_children(dst);
	if (ctree.empty()) {
	  (*dst)=create_false<T>(); //contradiction
	} else if (ctree.begin().begin()==ctree.begin().end() && 
		   guard_set(*ctree.begin()).empty()) {
	  (*dst)=create_true<T>();  //tautology
	} else {
	  _tr=&tr;
	  if (guard_set(*ctree.begin()).empty() && 
	      ctree.begin().number_of_children()==1)
	    rec_inv_make(ctree.begin().begin(),dst);
	  else
	    rec_inv_make(ctree.begin(),dst);
	  /*rec_inv_make(ctree.begin(),dst);*/
	}
      }
    private:
      const AndT& is_and;
      const OrT is_or;
      mutable C2T _c2t;
      mutable T2C _t2c;
      mutable tree<T>* _tr;
      mutable Tree _ctr;

      Constraint to_constraint(tsib_it it) const {
	typename T2C::iterator loc=_t2c.find(it);
	return (loc!=_t2c.end()) ? loc->second : _t2c.insert
	  (make_pair(it,_c2t.insert
		     (make_pair(_c2t.size()+1,
				it)).first->first)).first->second;
      }
      tsib_it from_constraint(Constraint c) const {
	assert(_c2t.find(c)!=_c2t.end());
	return _c2t.find(c)->second;
      }
      
      
      void rec_make(tsib_it src,sib_it dst) const { 
	using namespace std; 
	using namespace boost;

	if (is_not(*src)) {
	  *dst=create_and(-to_constraint(src.begin()));
	} else if (is_or(*src)) {
	  *dst=create_or<Node>();
	  _ctr.append_children(dst,src.number_of_children());
	  for_each(make_counting_iterator(src.begin()),
		   make_counting_iterator(src.end()),
		   make_counting_iterator(dst.begin()),
		   bind(&ConstraintCalculus::rec_make,this,_1,_2));
	} else if (is_and(*src)) {
	  *dst=create_and<Node>();
	  for (tsib_it child=src.begin();child!=src.end();++child)
	    if (is_not(*child))
	      guard_set(*dst).insert(-to_constraint(child.begin()));
	    else if (!is_and(*child) && !is_or(*child))
	      guard_set(*dst).insert(to_constraint(child));
	    else
	      rec_make(child,_ctr.append_child(dst));
	} else {
	  *dst=create_and(to_constraint(src));
	}
      }

      void rec_inv_make(sib_it src,tsib_it dst) const {
	using namespace std; 
	using namespace boost;

	_tr->append_children(dst,src.number_of_children());
	for_each(make_counting_iterator(src.begin()),
		 make_counting_iterator(src.end()),
		 make_counting_iterator(dst.begin()),
		 bind(&ConstraintCalculus::rec_inv_make,this,_1,_2));
	if (id::is_and(*src)) {
	  *dst=create_and<T>();
	  for (Set::const_iterator c=guard_set(*src).begin();
	       c!=guard_set(*src).end();++c) {
	    assert(*c!=0);
	    if (*c>0)
	      _tr->append_child(dst,from_constraint(*c));
	    else
	      _tr->append_child(_tr->append_child(dst,create_not<T>()),
				from_constraint(-(*c)));
	  }
	  if (dst.number_of_children()==1) {
	    *dst=*dst.begin();
	    _tr->erase(_tr->flatten(dst.begin()));
	  }
	} else {
	  assert(id::is_or(*src));
	  assert(src.begin()!=src.end());
	  *dst=create_or<T>();
	}
      }
    };
  } //~namespace enf
  
  template<typename T,typename It,typename AndT,typename OrT>
  void reduce_subtree_to_enf(tree<T>& tr,It src,
			     const AndT& is_and,const OrT& is_or) {
    enf::ConstraintCalculus<T,AndT,OrT> cc(is_and,is_or);
    enf::Tree ctree;
    //cout << "in : " << tr << endl;
    cc.to_constraint_tree(tr,src,ctree);
    constructor<enf::reduce_to_elegance>()(ctree);

    tree<T> tmp(*tr.begin());
    cc.from_constraint_tree(ctree,tmp,tmp.begin());
    //cout << tree<T>(src) << "PP" << tr << endl;
    src=tr.move_ontop(src,tmp.begin());
    //cout << tree<T>(src) << "PP" << tr << endl;
    canonical_order(tr,src);
    //cout << "out : " << tr << endl;
  }

  template<typename T>
  void reduce_tree_to_enf(tree<T>& tr) {
    reduce_subtree_to_enf(tr,tr.begin(),
			  bind(id::is_and<T>,_1),
			  bind(id::is_or<T>,_1));
  }

} //~namespace rewrite

std::ostream& operator<<(std::ostream& out,const rewrite::enf::Node& x);

#endif
