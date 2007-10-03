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

#include "general_rules.h"
#include "combo/eval.h"
#include "combo/assumption.h"

namespace reduct {
  typedef vtree::sibling_iterator sib_it;

  //flattens all associative functions: f(a,f(b,c)) -> f(a,b,c)
  //note that level is recursive that is f(a,f(b,f(c,d))) -> f(a,b,c,d)
  void level::operator()(vtree& tr,vtree::iterator it) const {
    if (is_associative(*it))
      for (sib_it sib=it.begin();sib!=it.end();)
	if (*sib==*it)
	  sib=tr.erase(tr.flatten(sib));
	else
	  ++sib;
  }

  //evaluates sub-expressions when possible
  //if an operator is commutative, op(const,var,const) will become
  //op(op(const,const),var), e.g., +(2,x,1)->+(3,x)
  void eval_constants::operator()(vtree& tr,vtree::iterator it) const {
    if (it.is_childless())
      return;
    sib_it to;
    if (!is_commutative(*it)) {
      for (sib_it sib=it.begin();
	   sib!=it.end();++sib)
	if (!is_constant(*sib))
	  return;
    } else {
      //needs to be deep, so can't call std::partition
      to=tr.partition(it.begin(),it.end(),is_constant<vertex>);
      int n_consts=distance(it.begin(),to);
      if (n_consts<2 && (!(n_consts==1 && it.has_one_child())))
	return;
      if (to!=it.end()) {
	tr.reparent(tr.append_child(it,*it),it.begin(),to);
	it=it.last_child();
      }
    }
    *it=eval_throws(it);
    tr.erase_children(it);
  }

  //Reorder children of commutative operators (should be applied upwards)
  void reorder_commutative::operator()(vtree& tr,vtree::iterator it) const {
    if(is_commutative(*it))
      tr.sort_on_subtrees(it.begin(),it.end(),
			  util::lexicographic_subtree_order<vertex>(),false);
  }

  //Get rid of subtrees marked with a null_vertex in their roots
  void remove_null_vertices::operator()(vtree& tr,vtree::iterator it) const {
    for (sib_it sib=it.begin();sib!=it.end();)
      if (*sib==id::null_vertex)
	sib=tr.erase(sib);
      else
	++sib;
  }

  void remove_all_assumptions::operator()(vtree& tr,vtree::iterator it) const {
    delete_all_assumptions(tr);
  }

} //~namespace reduct

