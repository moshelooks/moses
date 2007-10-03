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

#ifndef _META_RULES_IMPL_H
#define _META_RULES_IMPL_H

#include "reduct/meta_rules.h"
#include "util/foreach.h"
#include "combo/assumption.h"

namespace reduct {

  void downwards::operator()(vtree& tr,vtree::iterator it) const {
    vtree::iterator end=it;  
    end.skip_children();
    ++end;

    if (input==combo::id::unknown)
      for (;it!=end;++it)
	(*r)(tr,it);
    else
      for (;it!=end;++it)
	if ((combo::vertex_input_type(*it)==input ||
	     combo::vertex_input_type(*it)==combo::id::unknown) && 
	    combo::vertex_output_type(*it)==output)
	    (*r)(tr,it);
  }

  //apply rule from the leaves of the subtree rooted by it to it
  void upwards::operator()(vtree& tr,vtree::iterator it) const {
    vtree::post_order_iterator at=it,end=it;
    ++end;
    at.descend_all();

    for (;at!=end;++at)
      (*r)(tr,at);
  }

  void sequential::operator()(vtree& tr,vtree::iterator it) const {
    foreach (const rule& r,rules)
      r(tr,it);
  }

  void iterative::operator()(vtree& tr,vtree::iterator it) const {
    vtree tmp;
    do {
      tmp=vtree(it);
      (*r)(tr,it);
    } while (!tr.equal_subtree(it,tmp.begin()));
  }

  void assum_iterative::operator()(vtree& tr,vtree::iterator it) const {
    vtree tmp;
    do {
      tmp = tr;
      (*r)(tr,it);
    } while(tr!=tmp || !equal_assumptions(tmp, tr));
  }

} //~namespace reduct

#endif
