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

#include "eda/local_structure.h"
#include "util/foreach.h"
#include "util/numeric.h"
#include "util/algorithm.h"
#include "util/selection.h"

namespace eda {
  //true if the range is uniform on the variable at index idx
  bool local_structure_model::is_uniform_on(iptr_iter l,iptr_iter u,int idx) {
    return (adjacent_find(make_indirect_iterator(l),make_indirect_iterator(u),
			  bind(std::not_equal_to<disc_t>(),
			       bind(&field_set::get_raw,&_fields,_1,idx),
			       bind(&field_set::get_raw,&_fields,_2,idx)))==
	    make_indirect_iterator(u));
  }

  void local_structure_model::rec_split_onto(iptr_iter l,iptr_iter u,
					     int src_idx,int idx,
					     dtree::iterator node,
					     onto_tree::iterator osrc) 
  {
    int raw_arity=osrc.number_of_children()+1;
    
    if (is_uniform_on(l,u,idx) || raw_arity==1) {
      //make us a leaf
      *node=dtree_node(raw_arity+1,0);
      return;
    }

    assert(node->size()==1);
    (begin()+idx)->append_children(node,*node,raw_arity);
    node->front()=src_idx;
    (*node.begin())=dtree_node(2,0);

    if (src_idx<idx-1) { //important - needs to match onto_spec::Left/Right
      vector<iptr_iter> pivots(raw_arity);
      pivots.back()=u;
      util::n_way_partition
	(l,u,bind(&field_set::get_raw,&_fields,
		  bind(util::valueof<const instance>,_1),src_idx),
	 raw_arity,pivots.begin());

      //we don't recurse on the leftmost partition - it's onto_spec::Stop
      //instead just make it a (null) leaf 
      for_each(pivots.begin(),--pivots.end(),++pivots.begin(),
	       make_counting_iterator(++node.begin()),	       
	       make_counting_iterator(osrc.begin()),
	       bind(&local_structure_model::rec_split_onto,this,_1,_2,
		    src_idx+1,idx,_3,_4));
    } else {
      //we're the parent of leaves
      dtree::sibling_iterator dsib=++node.begin();
      for (onto_tree::sibling_iterator osib=osrc.begin();osib!=osrc.end();
	   ++osib,++dsib) {
	*dsib=dtree_node(raw_arity+1,0);
      }
    }
  }

  void local_structure_model::rec_split_contin
  (iptr_iter l,iptr_iter u,int src_idx,int idx,dtree::iterator node) {

    if (adjacent_find(make_indirect_iterator(l),make_indirect_iterator(u),
		      bind(std::not_equal_to<disc_t>(),
			   bind(&field_set::get_raw,&_fields,_1,idx),
			   bind(&field_set::get_raw,&_fields,_2,idx)))==
	make_indirect_iterator(u))
      return;

    split(src_idx,idx,node);

    if (src_idx<idx-1) { //important - needs to match contin_spec::Left/Right
      vector<iptr_iter> pivots(2);
      util::n_way_partition
	(l,u,bind(&field_set::get_raw,&_fields,
		  bind(util::valueof<const instance>,_1),src_idx),
	 3,pivots.begin());
      rec_split_contin(pivots[0],pivots[1],src_idx+1,idx,++node.begin());
      rec_split_contin(pivots[1],u,src_idx+1,idx,node.last_child());
    }
  }

  void local_structure_model::make_dtree(super::iterator dtr,int arity) {
    dtr->set_head(dtree_node(arity+1,0));
  }	
  
  void local_structure_model::split(int src_idx,int tgt_idx,
				    dtree::iterator tgt) {
    assert(tgt.number_of_children()==0);
    (begin()+tgt_idx)->append_children(tgt,*tgt,tgt->size()-1);
    *tgt=dtree_node(1,src_idx);
  }
  
  //sample from a model
  instance local_structure_model::operator()() const {
    vector<disc_t> tmp(size());
    foreach(unsigned int idx,_ordering) {
      sample((begin()+idx)->begin(),tmp[idx],tmp);
    }
    instance res(_instance_length);
    _fields.pack(tmp.begin(),res.begin());

    return res;
  }

  void local_structure_model::sample(dtree::iterator dtr,disc_t& dst,
				     const vector<disc_t>& margs) const {
    if (dtr.is_childless()) { //a leaf
      if (dtr->back()>0)
	dst=distance(dtr->begin(),
		     util::roulette_select(dtr->begin(),--(dtr->end()),
					   dtr->back()));
      else
	dst=0;//util::randint(dtr->size()-1); //if no data, do uniform selection
    } else {
      assert(dtr->size()==1);
      assert((int)dtr.number_of_children()>margs[dtr->front()]);
      dtree::sibling_iterator child=dtr.begin();
      child+=margs[dtr->front()];
      sample(child,dst,margs);
    }
  }

} //~namespace eda
