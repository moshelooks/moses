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

#include "action.h"

std::ostream& operator<<(std::ostream& out,const combo::action& a) {
  using namespace combo;
  switch(a) {
  case id::sequential_and:
    return out << "and_seq";
  case id::sequential_or:
    return out << "or_seq";
  case id::sequential_exec:
    return out << "exec_seq";
  case id::boolean_action_if:
    return out << "boolean_action_if";
  case id::contin_action_if:
    return out << "contin_action_if";
  case id::action_action_if:
    return out << "action_action_if";
  case id::action_failure:
    return out << "failure";
  case id::action_success:
    return out << "success";
  case id::dummy_action:
    return out << "dummy_action";
  default:
    return out << "ACTION : UNKNOWN_HANDLE";  
  }
}
