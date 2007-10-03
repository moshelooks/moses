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

#ifndef _REDUCT_PERCEPTION_RULES_H
#define _REDUCT_PERCEPTION_RULES_H

#include "reduct/reduct.h"
#include "combo/tree_type.h"

namespace reduct {
  
  //add in the set of assumptions such knowledge
  //f(x, z) <= max{d(x, y), d(y, z)}
  //when encounting f(x, y) and f(y, z) and d is ultrametric
  struct reduce_ultrametric : public crule<reduce_ultrametric> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };
 
  //add in the set of assumptions such knowledge
  //f(x, z)
  //when encounting f(x, y) and f(y, z)
  struct reduce_transitive : public crule<reduce_transitive> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //reduce f(x,x) -> true iff f is reflexive
  struct reduce_reflexive : public crule<reduce_reflexive> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //reduce f(x,x) -> false iff f is irreflexive
  struct reduce_irreflexive : public crule<reduce_irreflexive> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //reduce f(x,y) -> 0 iff f verifies the property of identity of indiscernibles
  struct reduce_identity_of_indiscernibles : public crule<reduce_identity_of_indiscernibles> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

}

#endif
