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

#ifndef _COMBO_PERCEPTION_EVAL_H
#define _COMBO_PERCEPTION_EVAL_H

#include "combo/vertex.h"
#include "util/exception.h"
#include "combo/using.h"
#include "combo/tree_type.h"
#include "util/tree.h"


namespace combo {

  template<typename It>
  vertex action_eval_throws(It it) throw(EvalException) {
    typedef typename It::sibling_iterator sib_it;
    //argument
    /*if(is_argument(*it)) {
      int idx=get_argument(*it).idx;
      //assumption : when idx is negative the argument is necessary boolean
      assert(idx>0);
      return binding(idx);
      }*/
    //perception
    if(*it==id::dummy_ultrametric) {
      return 0.0
    }
    else if(*it==id::dummy_transitive) {
      return id::logical_true;
    }
  }

}

#endif
