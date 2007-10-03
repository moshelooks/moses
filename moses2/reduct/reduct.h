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

#ifndef _REDUCT_RULE_H
#define _REDUCT_RULE_H

#include "reduct/using.h"

namespace reduct {

  struct rule {
    virtual ~rule() { }
    virtual void operator()(vtree&,vtree::iterator) const=0;
    virtual rule* clone() const=0;

    void operator()(vtree& tr) const { 
      if (!tr.empty())
	(*this)(tr,tr.begin());
    }
  };
  reduct::rule* new_clone(const reduct::rule& r);

  template<typename T>
  struct crule : public rule {
    rule* clone() const { return new T(*((T*)this)); }
  };

  const rule& logical_reduction();
  const rule& contin_reduction();
  const rule& mixed_reduction();
  const rule& full_reduction();
  const rule& action_reduction();
  const rule& perception_reduction();

  const rule& clean_reduction();
  //const rule& clean_and_full_reduction();

  inline void logical_reduce(vtree& tr,vtree::iterator it) {
    logical_reduction()(tr,it);
  }
  inline void logical_reduce(vtree& tr) { logical_reduction()(tr); }

  inline void contin_reduce(vtree& tr,vtree::iterator it) {
    contin_reduction()(tr,it);
  }
  inline void contin_reduce(vtree& tr) { contin_reduction()(tr); }

  inline void mixed_reduce(vtree& tr,vtree::iterator it) {
    mixed_reduction()(tr,it);
  }
  inline void mixed_reduce(vtree& tr) { mixed_reduction()(tr); }

  inline void full_reduce(vtree& tr,vtree::iterator it) {
    full_reduction()(tr,it);
  }
  inline void full_reduce(vtree& tr) { full_reduction()(tr); }

  inline void clean_reduce(vtree& tr,vtree::iterator it) {
    clean_reduction()(tr,it);
  }
  inline void clean_reduce(vtree& tr) { clean_reduction()(tr); }

  inline void clean_and_full_reduce(vtree& tr,vtree::iterator it) {
    //clean_and_full_reduction()(tr,it);
    clean_reduce(tr,it);
    full_reduce(tr,it);
  }
  inline void clean_and_full_reduce(vtree& tr) { 
    //clean_and_full_reduction()(tr,tr.begin()); 
    clean_reduce(tr);
    full_reduce(tr);
  }

  //action
  inline void action_reduce(vtree& tr, vtree::iterator it) {
    action_reduction()(tr,it);
  }

  inline void action_reduce(vtree& tr) { action_reduction()(tr); }

  //perception
  inline void perception_reduce(vtree& tr, vtree::iterator it) {
    perception_reduction()(tr,it);
  }

  inline void perception_reduce(vtree& tr) { perception_reduction()(tr); }

} //~namespace reduct

#endif
