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

#ifndef _REDUCT_GENERAL_RULES_H
#define _REDUCT_GENERAL_RULES_H

#include "reduct/reduct.h"

namespace reduct {

  //flattens all associative functions: f(a,f(b,c)) -> f(a,b,c)
  //note that level is recursive that is f(a,f(b,f(c,d))) -> f(a,b,c,d)
  struct level : public crule<level> { 
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //evaluates sub-expressions when possible
  //if an operator is commutative, op(const,var,const) will become
  //op(op(const,const),var), e.g., +(2,x,1)->+(3,x)
  struct eval_constants : public crule<eval_constants> { 
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //Reorder children of commutative operators (should be applied upwards)
  struct reorder_commutative : public crule<reorder_commutative> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //Get rid of subtrees marked with a null_vertex in their roots
  struct remove_null_vertices : public crule<remove_null_vertices> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //Remaove all assumptions
  struct remove_all_assumptions : public crule<remove_null_vertices> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

} //~namespace reduct

#endif
