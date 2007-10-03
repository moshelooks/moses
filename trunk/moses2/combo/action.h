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

#ifndef _COMBO_ACTION_H
#define _COMBO_ACTION_H

//#include "combo/vertex.h"
#include <iostream>

namespace combo {
  
  namespace id {
    enum action { 
      sequential_and, sequential_or, sequential_exec,
      boolean_action_if, contin_action_if, action_action_if,
      action_success, action_failure,
      dummy_action, //dummy elementary action to be replaced later
      action_count //to give the number of actions
    };
  }
  typedef id::action action;

  //inline bool is_action(const vertex& v) {
  //  return (boost::get<action>(&v));
  //}
  
  //inline action get_action(const vertex& v) {
  //  return (boost::get<action>(v));
  //}

}//~namespace combo

std::ostream& operator<<(std::ostream&,const combo::action&);

//inline bool operator==(const combo::vertex& v,combo::action a) {
//  if (const combo::action* va=boost::get<combo::builtin>(&v))
//    return (*va==a);
//  return false;
//}
//inline bool operator==(combo::action a,const combo::vertex& v) {
//  return (v==a);
//}
//inline bool operator!=(const combo::vertex& v,combo::action a) {
//  return !(v==a);
//}
//inline bool operator!=(combo::action a,const combo::vertex& v) {
//  return !(v==a);
//}

#endif
