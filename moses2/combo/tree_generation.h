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

#ifndef TREE_GENERATION_H
#define TREE_GENERATION_H

#include "util/tree.h"
#include "combo/vertex.h"
#include "combo/tree_type.h"

namespace trees {

  template<int MIN_ARITY>
  class Generator {
  public:

    template<typename Selector>
    util::tree<typename Selector::value_type> operator()(const Selector& sel,
						   int dp) const {
      typename Selector::value_type tmp;
      util::tree<typename Selector::value_type> tr(tmp);
      build(sel,dp,tr,tr.begin());
      return tr;
    }

    template<typename Selector,typename T,typename iterator>
    void build(const Selector& sel,int dp,util::tree<T>& tr,iterator it) const {
      if (dp==1) {
	tr.replace(it,sel.select(0));
      } else {
	int arity(sel.select_arity(MIN_ARITY));
	it=tr.replace(it,sel.select(arity));
	--dp;
	for (int i=0;i<arity;++i)
	  build(sel,dp,tr,tr.append_child(it));
      }
    }
  };

  typedef Generator<1> FullGenerator;
  typedef Generator<0> GrowGenerator;

  template<typename It,typename Selector>
  void ramped_half_and_half(It from,It to,const Selector& sel,
			    int minDp,int maxDp,bool type_check_enabled) {
    GrowGenerator gg;
    FullGenerator fg;
    bool gg_turn = true;
    bool well_typed;
    float ratio=((float)(maxDp-minDp+1))/(float)distance(from,to);
    for (It tr=from;tr!=to;) {
      (*tr)=(gg_turn?gg(sel,minDp+(int)(distance(from,tr)*ratio))
	     :fg(sel,minDp+(int)(distance(from,tr)*ratio)));
      well_typed = true;
      if(type_check_enabled) {
	std::stringstream strs;
	combo::vtree vtr;
	strs << (*tr);
	strs >> vtr;
	try {
	  combo::infer_tree_type(vtr);
	}
	catch(TypeCheckException) {
	  well_typed = false;
	}
      }
      if(well_typed) {
	++tr;
	gg_turn = !gg_turn;
      }
    }
  }

} //~namespace trees

#endif
