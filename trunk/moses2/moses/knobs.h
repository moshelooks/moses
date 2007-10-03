/****
   Copyright 2005-2007, Moshe Looks and Novamente LLC

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

#ifndef _MOSES_KNOBS_H
#define _MOSES_KNOBS_H

#include "util/tree.h"
#include "moses/using.h"
#include <bitset>
#include "reduct/reduct.h"
#include "util/based_variant.h"
#include "util/numeric.h"
#include "combo/complexity.h"
#include "eda/field_set.h"

namespace moses {

  //a knob represents a single dimension of variation relative to an exemplar
  //program tree. This may be discrete or continuous. In the discrete case, the
  //various settings are accessible via turn(0),turn(1),...turn(arity()-1). In
  //the continuous case, turn(contin_t) is used.
  //
  //for example, given the program tree fragment or(0<(*(#1,0.5)),#2), a
  //continuous knob might be used to vary the numerical constant. So setting
  //this knob to 0.7 would transform the tree fragment to
  //or(0<(*(#1,0.7)),#2). A discrete knob with arity()==3 might be used to
  //transform the boolean input #2. So setting this knob to 1 might transform
  //the tree to or(0<(*(#1,0.7)),not(#2)), and setting it to 2 might remove it
  //from the tree (while setting it to 0 would return to the original tree).

  struct knob_base {
    knob_base(vtree& tr) : _tr(&tr) { }
    virtual ~knob_base() { }

    //is the feature nonzero by default? i.e., is it present in the exemplar?
    virtual bool in_exemplar() const=0;

    //return the exemplar to its state before the knob was created (deleting
    //any null vertices if present)
    virtual void clear_exemplar()=0;
  protected:
    vtree* _tr;
  };

  struct disc_knob_base : public knob_base {
    disc_knob_base(vtree& tr) : knob_base(tr) { }
    virtual ~disc_knob_base() { }

    virtual void turn(int)=0;
    virtual void disallow(int)=0;
    virtual void allow(int)=0;

    //create a spec describing the space spanned by the knob
    virtual eda::field_set::disc_spec spec() const=0;

    //arity based on whatever knobs are currently allowed
    virtual int arity() const=0;

     //expected complexity based on whatever the knob is currently turned to
    virtual int complexity_bound() const=0;
  };

  struct contin_knob : public knob_base {
    contin_knob(vtree& tr,vtree::iterator tgt,
		contin_t step_size,contin_t expansion,eda::field_set::arity_t depth)
      : knob_base(tr),_tgt(tgt),_spec(combo::get_contin(*tgt),
				      step_size,expansion,depth) { }

    bool in_exemplar() const { return true; }
    void clear_exemplar() { }

    void turn(eda::contin_t x) { *_tgt=x; }

    //create a spec describing the space spanned by the knob
    const eda::field_set::contin_spec& spec() const { return _spec; }
  protected:
    vtree::iterator _tgt;
    eda::field_set::contin_spec _spec;
  };

  template<int MaxArity>
  struct knob_with_arity : public disc_knob_base {
    knob_with_arity(vtree& tr) : disc_knob_base(tr),_default(0) { }

    void disallow(int idx) { _disallowed[idx]=true; }
    void allow(int idx) { _disallowed[idx]=false; }

    int arity() const { return MaxArity-_disallowed.count(); }

    bool in_exemplar() const { return (_default!=0); }
  protected:
    std::bitset<MaxArity> _disallowed;
    int _default;

    int map_idx(int idx) const { 
      if (idx==_default)
	idx=0;
      else if (idx==0)
	idx=_default;
      return idx+(_disallowed<<(MaxArity-idx)).count();
    }
  };

  //note - children aren't cannonized when parents are called
  struct logical_subtree_knob : public knob_with_arity<3> { 
    static const int absent=0;
    static const int present=1;
    static const int negated=2;

    logical_subtree_knob(vtree& tr,vtree::iterator tgt,vtree::iterator subtree)
      : knob_with_arity<3>(tr),_current(absent),_loc(tr.end()) {

      //compute the negation of the subtree
      vtree negated_subtree(subtree);
      negated_subtree.insert_above(negated_subtree.begin(),id::logical_not);
      reduct::logical_reduce(negated_subtree);

      for (vtree::sibling_iterator sib=tgt.begin();sib!=tgt.end();++sib) {
	if (_tr->equal_subtree(vtree::iterator(sib),subtree) || 
	    _tr->equal_subtree(vtree::iterator(sib),negated_subtree.begin())) {
	  _loc=sib;
	  _current=present;
	  _default=present;
	  return;
	}
      }

      _loc=_tr->append_child(tgt,id::null_vertex);
      _tr->append_child(_loc,subtree);
    }

    int complexity_bound() const { 
      return (_current==absent ? 0 : combo::complexity(_loc));
    }

    void clear_exemplar() { 
      if (in_exemplar())
	turn(0);
      else
	_tr->erase(_loc);
    }

    void turn(int idx) {
      idx=map_idx(idx);
      assert(idx<3);

      if (idx==_current) //already set, nothing to 
	return;

      switch(idx) {
      case absent:
	//flag subtree to be ignored with a null_vertex, replace negation if present
	if (_current==negated)
	  *_loc=id::null_vertex;
	else
	  _loc=_tr->insert_above(_loc,id::null_vertex);
	break;
      case present:
	_loc=_tr->erase(_tr->flatten(_loc));
	break;
      case negated:
	if (_current==present)
	  _loc=_tr->insert_above(_loc,id::logical_not);
	else
	  *_loc=id::logical_not;
	break;
      }
      
      _current=idx;
    }

    eda::field_set::disc_spec spec() const {
      return eda::field_set::disc_spec(arity());
    }


  protected:
    int _current;
    vtree::iterator _loc;
  };

  typedef util::based_variant<boost::variant<logical_subtree_knob>,
			      disc_knob_base> disc_knob;
} //~namespace moses

#endif
