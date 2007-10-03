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

#include "reduct/perception_rules.h"
#include "combo/assumption.h"

namespace reduct {
  typedef vtree::sibling_iterator sib_it;
  typedef vtree::iterator pre_it;

  void reduce_ultrametric::operator()(vtree& tr,vtree::iterator it) const {
    if(is_ultrametric(*it)) {
      assert(it.number_of_children()==2);
      std::vector<pre_it> it_vec;
      if(find_vertices_in_assumptions(tr, *it, it_vec)) {
	std::vector<pre_it> arg1;
	std::vector<pre_it> arg2;
	for(std::vector<pre_it>::iterator it_vec_it = it_vec.begin();
	    it_vec_it != it_vec.end(); ++it_vec_it) {
	  assert((*it_vec_it).number_of_children()==2);
	  arg1.push_back(pre_it(tr.child(*it_vec_it, 0)));
	  arg2.push_back(pre_it(tr.child(*it_vec_it, 1)));
	}
	for(unsigned int i2 = 0; i2 < arg2.size(); i2++) {
	  for(unsigned int i1 = 0; i1 < arg2.size(); i1++) {
	    if(i1 != i2 && tr.equal_subtree(arg1[i2], arg2[i1])) {
	      //must build and add 0<f(a,c)-f(a,b) and 0<f(a,c)-f(b,c)
	      //that is :
	      //build and add the two new assumptions
	      //0<*it(arg1[i1],arg2[i2])-*it(arg1[i1],arg2[i1])
	      //0<*it(arg1[i1],arg2[i2])-*it(arg1[i2],arg2[i2])
	      {
		vtree new_a1_tr;
		pre_it ait1 = new_a1_tr.set_head(id::greater_than_zero);
		pre_it pl1 = new_a1_tr.append_child(ait1, id::plus);
		{ //create *it(arg1[i1],arg2[i2])
		  pre_it it1 = new_a1_tr.append_child(pl1, *it);
		  tr.replace(tr.append_child(it1), arg1[i1]);
		  tr.replace(tr.append_child(it1), arg2[i2]);
		}
		{ //create -*it(arg1[i1],arg2[i1])
		  pre_it new_times = new_a1_tr.append_child(pl1, id::times);
		  new_a1_tr.append_child(new_times, -1.0);
		  pre_it it2 = new_a1_tr.append_child(new_times, *it);
		  tr.replace(tr.append_child(it2), arg1[i1]);
		  tr.replace(tr.append_child(it2), arg2[i1]);
		}		  
		//insert first assumption
		insert_assumption(tr, ait1);
	      }
	      {
		vtree new_a2_tr;
		pre_it ait2 = new_a2_tr.set_head(id::greater_than_zero);
		pre_it pl2 = new_a2_tr.append_child(ait2, id::plus);
		{ //create *it(arg1[i1],arg2[i2])
		  pre_it it1 = new_a2_tr.append_child(pl2, *it);
		  tr.replace(tr.append_child(it1), arg1[i1]);
		  tr.replace(tr.append_child(it1), arg2[i2]);
		}
		{ //create -*it(arg1[i2],arg2[i2])
		  pre_it new_times = new_a2_tr.append_child(pl2, id::times);
		  new_a2_tr.append_child(new_times, -1.0);
		  pre_it it2 = new_a2_tr.append_child(new_times, *it);
		  tr.replace(tr.append_child(it2), arg1[i2]);
		  tr.replace(tr.append_child(it2), arg2[i2]);
		}		  
		//insert first assumption
		insert_assumption(tr, ait2);
	      }
	    }
	  }
	}
      }
    }
  }

  void reduce_transitive::operator()(vtree& tr,vtree::iterator it) const {
    if(is_transitive(*it)) {
      assert(it.number_of_children()==2);
      pre_it main_tree = tr.begin();
      sib_it assum = tr.next_sibling(main_tree);
      if(tr.is_valid(assum)) {
	std::vector<pre_it> arg1;
	std::vector<pre_it> arg2;
	for(;assum != tr.end(); ++assum) {
	  if(*assum==*it) {
	    assert(assum.number_of_children()==2);
	    arg1.push_back(pre_it(tr.child(assum, 0)));
	    arg2.push_back(pre_it(tr.child(assum, 1)));
	  }
	}
	if(!arg1.empty() /*or arg2 not empty*/) {
	  for(unsigned int i2 = 0; i2 < arg2.size(); i2++) {
	    for(unsigned int i1 = 0; i1 < arg2.size(); i1++) {
	      if(i1 != i2 && tr.equal_subtree(arg1[i2], arg2[i1])) {
		//build new assumption *it(arg1[i1], arg2[i2])
		vtree new_assum_tr;
		pre_it na_it = new_assum_tr.set_head(*it);
		tr.replace(new_assum_tr.append_child(na_it), arg1[i1]);
		tr.replace(new_assum_tr.append_child(na_it), arg2[i2]);
		//insert new assumption
		insert_assumption(tr, na_it);
	      }
	    }
	  }
	}
      }
    }
  }

  void reduce_reflexive::operator()(vtree& tr,vtree::iterator it) const {
    if(is_reflexive(*it)) {
      assert(it.number_of_children()==2);
      if(tr.equal_subtree(pre_it(tr.child(it, 0)), pre_it(tr.child(it, 1)))) {
	*it = id::logical_true;
	tr.erase_children(it);
      }
    }
  }

  void reduce_irreflexive::operator()(vtree& tr,vtree::iterator it) const {
    if(is_irreflexive(*it)) {
      assert(it.number_of_children()==2);
      if(tr.equal_subtree(pre_it(tr.child(it, 0)), pre_it(tr.child(it, 1)))) {
	*it = id::logical_false;
	tr.erase_children(it);
      }
    }
  }

  void reduce_identity_of_indiscernibles::operator()(vtree& tr,vtree::iterator it) const {
    if(is_identity_of_indiscernibles(*it)) {
      assert(it.number_of_children()==2);
      if(tr.equal_subtree(pre_it(tr.child(it, 0)), pre_it(tr.child(it, 1)))) {
	*it = 0.0;
	tr.erase_children(it);
      }
    }
  }

}
