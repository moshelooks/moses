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

#ifndef _REDUCT_ACTION_RULES_H
#define _REDUCT_ACTION_RULES_H

#include "reduct/reduct.h"

namespace reduct {

  //action_action_if(A B B) -> and_seq(exec_seq(A) B)
  struct reduce_action_action_if : public crule<reduce_action_action_if> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //T_action_if(action_failure A B) -> B
  //T_action_if(action_failure A B) -> A
  //with T in {action, boolean, contin}
  struct reduce_const_cond_action_if:public crule<reduce_const_cond_action_if>
  {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //and_seq(A action_failure B) -> and_seq(A action_failure)
  //and_seq(A action_success B) -> and_seq(A B)
  //or_seq(A action_failure B) -> or_seq(A B)
  //or_seq(A action_success B) -> or_seq(A action_success)
  //exec_seq(A action_failure B) -> exec_seq(A B)
  //exec_seq(A action_success C) -> exec_seq(A B)
  struct reduce_const_action_seq : public crule<reduce_const_action_seq>
  {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //and_seq() -> action_success
  //or_seq() -> action_failure
  //exec_seq() -> action_success
  struct reduce_empty_arg_seq : public crule<reduce_empty_arg_seq>
  {
    void operator()(vtree& tr,vtree::iterator it) const;
  };
  
  //nullary
  //reduce any consecutive actions
  //TODO

}

#endif
