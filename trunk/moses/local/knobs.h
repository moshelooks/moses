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

#ifndef KNOBS_H
#define KNOBS_H

#include "tree.h"
#include "selection.h"
//#include "dtree_modeling.h"
#include "Vertex.h"
#include <iostream>
#include "tree_util.h"
//#include "semantic_sampler.h"
#include <boost/iterator/counting_iterator.hpp>

namespace local {
  //using namespace modeling;
  using namespace trees;
  using namespace boost;
  using namespace __gnu_cxx;
  using namespace std;
  using namespace id;

  typedef vtree::pre_order_iterator pre_it;
  typedef vtree::sibling_iterator sib_it;
  typedef NodeSelector<Vertex>::node_iterator node_iterator;

  struct KnobBase {
    KnobBase(pre_it d=pre_it(NULL),pre_it s=pre_it(NULL))
      : dst(d),src(s),bad1(false),bad2(false) { }
    virtual ~KnobBase() { }
    virtual void turn(vtree& tr,int) const=0;
    virtual int arity() const=0;
    virtual int bound(int) const=0;

    virtual bool valid(int) const { return true; }
    
    void disallow(int idx) { //this implementation is hacky and must be fixed
      if (idx==1) 
	bad1=true;
      else if (idx==2)
	bad2=true;
      else
	assert(false);
    }
    bool turnable() const { return (!bad1 || !bad2); }
    int random_nonzero() const { 
      assert(turnable());
      return (arity()==2 ? 1 : (bad1 ? 2 : (bad2 ? 1 : randint(2)+1)));
    }
    pre_it dst;
    mutable pre_it src;
    bool bad1,bad2;

    static bool logical_singleton(pre_it it) {
      return (is_not(*it) ? logical_singleton(it.begin()) :
	      (++it.begin()==it.end() && !is_or(*it.begin())));
    }
    static bool logical_and(pre_it it) {
      return (is_not(*it) ? !logical_and(it.begin()) :
	      (!logical_singleton(it) && !is_or(*it.begin())));
    }
  };

  struct Flip : public KnobBase {
    Flip() { }
    Flip(pre_it d) : KnobBase(d),start_and(is_and(*d)) { }
    virtual ~Flip() { }
    bool start_and;
    
    void turn(vtree& tr,int i) const {
      if ((i==1 && start_and) || (i==0 && !start_and))
	*dst=create_or<Vertex>();
      else
	*dst=create_and<Vertex>();
    }
    int arity() const { return 2; }
    int bound(int) const { return 0; }
  };
  struct Child : public KnobBase {
    Child(pre_it d,pre_it s) : KnobBase(d,s),setting(0) { }
    virtual ~Child() { }
    mutable sib_it tmp;
    mutable int setting;

    bool valid(int i) const {
      if (logical_singleton(src))
	return true;
      return (i==1 ? (is_or(*dst) ? logical_and(src) : !logical_and(src)) :
	      (is_or(*dst) ? !logical_and(src) : logical_and(src)));
    }

    void turn(vtree& tr,int i) const {
      if (setting==i)
	return;
      if (i==1) { //normal add
	if (setting==2)
	  tmp=tr.erase(tr.flatten(tmp));
	else
	  tmp=tr.append_child(dst,src);
      } else if (i==2) {
	if (setting==0)
	  tmp=tr.append_child(dst,src);
	tmp=tr.insert_above(tmp,create_not<Vertex>());
      } else {
	tr.erase(tmp);
      }
      setting=i;
    }
    int arity() const { return 3; }
    int bound(int idx) const { return complexity(src)+1; }
  };

  struct Removal : public KnobBase {
    Removal(pre_it d,pre_it s)
      : KnobBase(d,s),tmp(Vertex()),setting(0),last(s==d.last_child()) { }
    virtual ~Removal() { }
    mutable vtree tmp;
    mutable int setting;
    bool last;

    void turn(vtree& tr,int i) const {
      if (setting==i)
	return;
      if (setting==1) { //it's already out
	if (i==2) //negate it first
	  tmp.insert_above(tmp.begin(),create_not<Vertex>());
	//now put it in
	src=(last ? tr.move_ontop(tr.append_child(dst),tmp.begin()) :
	     tmp.move_before(src,tmp.begin()));
	tmp.set_head(Vertex());
      } else if (i==1) { //we need to take it out
	sib_it nxt(src);
	++nxt;
	tmp.move_ontop(tmp.begin(),src);
	src=nxt;
	if (setting==2) //remove the negation
	  tmp.erase(tmp.flatten(tmp.begin()));
      } else {
	//we need to swap negation
	if (setting==2)
	  src=tr.erase(tr.flatten(src));
	else
	  src=tr.insert_above(src,create_not<Vertex>());
      }
      setting=i;
    }
    int arity() const { return 3; }
    int bound(int idx) const { 
      return -1-(idx==1 ? (setting==1 ? complexity(tmp) : 
			   complexity(src)) : -1);
    }
  };

  struct JoinKnob : public KnobBase {
    JoinKnob(pre_it d,int basearity)
      : KnobBase(d),defarg(get_argument_idx(*d)),
	nuarg(defarg==0 ? randint(basearity*2+1)-basearity : 0) { }
    virtual ~JoinKnob() { }
    //int basearity;
    int defarg;
    int nuarg;
    
    void turn(vtree& tr,int i) const {
      if (i==0)
	*dst=create_argument<Vertex>(defarg);
      else
	*dst=create_argument<Vertex>(nuarg);
	  
      /*if (i==0) {
	*dst=create_argument<Vertex>(defarg);
      } else {
	int idx=i>basearity ? basearity-i : i;
	if (idx==defarg)
	  *dst=create_argument<Vertex>(0);
	else
	  *dst=create_argument<Vertex>(idx);
	  }**/
    }
    int arity() const { return 2; }//(basearity*2+1); }
    int bound(int) const { return 0; }

    int random_nonzero() const { return 1; }
    //int random_nonzero() const { return randint(basearity*2)+1; }
  };

  struct AntMover : public KnobBase {
    AntMover(pre_it d)
      : KnobBase(d),onByDefault(is_move(*d)) { }
    virtual ~AntMover() { }
    bool onByDefault;
    
    void turn(vtree& tr,int i) const {
      if ((onByDefault && i==0) || (!onByDefault && i==1))
	*dst=create_move<Vertex>();
      else
	*dst=create_progn<Vertex>();
    }
    int arity() const { return 2; }
    int bound(int) const { return 0; }

    int random_nonzero() const { return 1; }
  };

  struct AntRotater : public KnobBase {
    AntRotater(pre_it d)
      : KnobBase(d),defIdx(is_progn(*d) ? 0 :
			   is_left(*d) ? 1 :
			   is_right(*d) ? 2 : 3) { }
    virtual ~AntRotater() { }
    int defIdx;
    
    void turn(vtree& tr,int i) const {
      if (i==0) {
	*dst=(defIdx==0 ? create_progn<Vertex>() :
	      defIdx==1 ? create_left<Vertex>() :
	      defIdx==2 ? create_right<Vertex>() : create_reversal<Vertex>());
      } else if (i==defIdx) {
	*dst=create_progn<Vertex>();
      } else {
	*dst=(i==1 ? create_left<Vertex>() :
	      i==2 ? create_right<Vertex>() : create_reversal<Vertex>());
      }
    }
    int arity() const { return 4; }
    int bound(int) const { return 0; }

    int random_nonzero() const { return randint(3)+1; }
  };

#if 0
  struct JoinPrevSibling : public KnobBase {
    JoinPrevSibling(pre_it d,pre_it s) : KnobBase(d,s),setting(0) { }
    virtual ~JoinPrevSibling() { }
    mutable sib_it tmp;
    mutable int setting;

    bool valid(int i) const { return true; }

    void turn(vtree& tr,int i) const {
      if (setting==i)
        return;
      if (i==1) { //normal add                                       
	tmp=tr.insert_subtree(dst,src);
      } else {
	assert(i==0);
	tr.erase(tmp);
      }
      setting=i;
    }
    int arity() const { return 2; }
    int bound(int idx) const { return 0; }
  };
  struct JoinAppendChild : public KnobBase {
    JoinAppendChild(pre_it d,pre_it s) : KnobBase(d,s),setting(0) { }
    virtual ~JoinAppendChild() { }
    mutable sib_it tmp;
    mutable int setting;

    bool valid(int i) const { return true; }

    void turn(vtree& tr,int i) const {
      if (setting==i)
        return;
      if (i==1) { //normal add                                       
	tmp=tr.append_child(dst,src);
      } else {
	assert(i==0);
	tr.erase(tmp);
      }
      setting=i;
    }
    int arity() const { return 2; }
    int bound(int idx) const { return 0; }
  };
  struct JoinRemoval : public KnobBase {
    JoinRemoval(pre_it d,pre_it s)
      : KnobBase(d,s),tmp(Vertex()),setting(0),last(s==d.last_child()) { }
    virtual ~JoinRemoval() { }
    mutable vtree tmp;
    mutable int setting;
    bool last;

    void turn(vtree& tr,int i) const {
      if (setting==i)
	return;
      if (setting==1) { //it's already out
	//now put it in
	src=(last ? tr.move_ontop(tr.append_child(dst),tmp.begin()) :
	     tmp.move_before(src,tmp.begin()));
	tmp.set_head(Vertex());
      } else if (i==1) { //we need to take it out
	sib_it nxt(src);
	++nxt;
	tmp.move_ontop(tmp.begin(),src);
	src=nxt;
      }
      setting=i;
    }
    int arity() const { return 2; }
    int bound(int idx) const { return 0; }
  };
#endif

  typedef variant<
    JoinKnob,
    AntRotater,AntMover,
    //JoinPrevSibling,JoinAppendChild,JoinRemoval
    Flip,Child,Removal> Knob;
  typedef vector<Knob> kseq;
  typedef tree<kseq> ktree;
  typedef ktree::pre_order_iterator kpre_it;
  typedef ktree::sibling_iterator ksib_it;
  
  struct KnobBaseGetter : public static_visitor<KnobBase*> {
    template<typename T>
    KnobBase* operator()(T& t) const { return &t; }
  };
  struct ConstKnobBaseGetter : public static_visitor<const KnobBase*> {
    template<typename T>
    const KnobBase* operator()(const T& t) const { return &t; }
  };
  KnobBase& get_knob(Knob& knob);
  const KnobBase& get_knob(const Knob& knob);

  int knob_count(kpre_it ks);

} //~namespace local

std::ostream& operator<<(std::ostream& out,const local::Knob& k);


#endif
