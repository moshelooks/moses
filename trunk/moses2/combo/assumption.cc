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

#include "assumption.h"
#include <iostream>
#include <vector>

namespace combo {
  typedef vtree::iterator pre_it;
  typedef vtree::sibling_iterator sib_it;

  void insert_assumption(vtree& tr, vtree::iterator assum_it) {
    typedef util::lexicographic_subtree_order<vertex> Comp;
    pre_it main_tree = tr.begin();
    assert(tr.is_valid(main_tree));
    int max_pos = tr.number_of_siblings(main_tree);
    if(max_pos>1) {
      int min_pos = 1;
      int diff = (max_pos-min_pos)/2;
      int cur_pos = min_pos + diff;
      sib_it cur_sib = main_tree;
      cur_sib += cur_pos;
      Comp comp;
      while(min_pos<max_pos) {
	if(comp(pre_it(cur_sib), assum_it)) {
	  min_pos = cur_pos+1;
	  diff = 1+(max_pos-min_pos)/2;
	  cur_pos += diff;
	  cur_sib += diff;
	}
	else if(comp(assum_it, pre_it(cur_sib))) {
	  max_pos = cur_pos-1;
	  diff = 1+(max_pos-min_pos)/2;
	  cur_pos -= diff;
	  cur_sib -= diff;
	}
	else return;
      }
      if(max_pos<min_pos)
	tr.insert_subtree_after(cur_sib, assum_it);
      else if(min_pos==max_pos) {
	if(comp(pre_it(cur_sib), assum_it)) {
	  tr.insert_subtree_after(cur_sib, assum_it);
	}
	else if(comp(assum_it, pre_it(cur_sib))) {
	  tr.insert_subtree(cur_sib, assum_it);
	}
      }
      else assert(false);
    }
    else tr.insert_subtree_after(main_tree, assum_it);
  }

  bool find_vertices_in_assumptions(const vtree& tr, vertex v,
				    std::vector<vtree::iterator>& res) {
    std::cout << "find : " << v << std::endl;
    pre_it tr_it = tr.begin();
    assert(tr.is_valid(tr_it));
    pre_it assum_it = tr.next_sibling(tr_it);
    bool is_there = false;
    if(tr.is_valid(assum_it)) {
      for(; assum_it != tr.end(); ++assum_it) {
	std::cout << "ASSUM : " << *assum_it << std::endl;
	if(*assum_it == v) {
	  res.push_back(assum_it);
	  is_there = true;
	}
      }
    }
    return is_there;
  }
  
  bool equal_assumptions(const vtree& tr1, const vtree& tr2) {
    assert(!tr1.empty() && !tr2.empty());
    sib_it assum1 = tr1.next_sibling(tr1.begin());
    sib_it assum2 = tr2.next_sibling(tr2.begin());
    if(tr1.is_valid(assum1) && tr2.is_valid(assum2)) {
      for(; assum1 != tr1.end() && assum2 != tr2.end(); ++assum1, ++assum2)
	if(!tr1.equal_subtree(assum1, assum2))
	  return false;
      if(assum1==tr1.end() && assum2==tr2.end())
	return true;
      else return false;
    }
    else return true;
  }

  void delete_all_assumptions(vtree& tr) {
    assert(!tr.empty());
    sib_it assum = tr.next_sibling(tr.begin());
    if(tr.is_valid(assum))
      for(; assum != tr.end();) 
	assum = tr.erase(assum);
  }

  bool test() {
    return true;
  }

}//~namespace combo
