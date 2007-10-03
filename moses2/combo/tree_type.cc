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

#include "combo/tree_type.h"

bool combo::is_boolean_output(const vertex& v) {
  using namespace combo;
  return (v==id::logical_and || v==id::logical_or || v==id::logical_not ||
          v==id::logical_true || v==id::logical_false ||
          v==id::greater_than_zero ||
	  //action
	  v==id::boolean_action_if ||
	  //perception
	  v==id::dummy_transitive || v==id::dummy_reflexive ||
	  v==id::dummy_irreflexive);
}

bool combo::is_contin_output(const vertex& v) {
  using namespace combo;
  return (v==id::contin_if || v==id::plus || v==id::times || v==id::div ||
	  v==id::log || v==id::exp || v==id::sin || v==id::impulse ||
	  is_contin(v) ||
	  //action
	  v==id::contin_action_if ||
	  //perception
	  v==id::dummy_ultrametric || v==id::dummy_identity_of_indiscernibles
	  );
}

//WARNING : should be action.h but could not do that due to dependency issues
bool combo::is_action_result_output(const vertex& v) {
  using namespace combo;
  return (v==id::sequential_and || v==id::sequential_or || 
	  v==id::sequential_exec || v==id::action_action_if ||
	  v==id::action_failure || v==id::action_success ||
	  v==id::dummy_action);
}

bool combo::is_boolean_input(const vertex& v) {
  using namespace combo;
  return (v==id::logical_and || v==id::logical_or || v==id::logical_not
	  || v==id::impulse);
}

bool combo::is_contin_input(const vertex& v) {
  using namespace combo;
  return (v==id::plus || v==id::times || v==id::div || v==id::log
	  || v==id::exp || v==id::sin || v==id::greater_than_zero);
}

//WARNING : should be action.h but could not do that due to dependency issues
bool combo::is_action_result_input(const vertex& v) {
  using namespace combo;
  return (v==id::sequential_and || v==id::sequential_or || 
	  v==id::sequential_exec || v==id::action_action_if);
}

combo::node_type combo::vertex_output_type(const vertex& v) {
  using namespace combo;
  if(is_boolean_output(v))
    return id::boolean;
  else if(is_contin_output(v))
    return id::contin;
  else if(is_action_result_output(v))
    return id::action_result;
  else return id::unknown;
}

combo::node_type combo::vertex_input_type(const vertex& v) {
  using namespace combo;
  if(is_boolean_input(v))
    return id::boolean;
  else if(is_contin_input(v))
    return id::contin;
  else if(is_action_result_input(v))
    return id::action_result;
  else return id::unknown;
}

combo::node_type combo::vertex_input_type(const vertex& v, int input_index) {
  if(v==id::contin_if) {
    if(input_index==0) return id::boolean;
    else return id::contin;
  }
  else if(v==id::boolean_action_if) {
    if(input_index==0) return id::action_result;
    else return id::boolean;
  }
  else if(v==id::contin_action_if) {
    if(input_index==0) return id::action_result;
    else return id::contin;
  }
  else return vertex_input_type(v);
}

int combo::contin_arity(const combo::tree_type& ty) {
  using namespace combo;
  typedef tree_type::sibling_iterator sib_it;
  int res = 0;
  tree_type::iterator ty_it = ty.begin();
  if(*ty_it==id::application)
    for(sib_it sib = ty_it.begin();sib != sib_it(ty.last_child(ty_it));++sib)
      if(*sib==id::contin)
	res++;
  return res;
}

int combo::boolean_arity(const combo::tree_type& ty) {
  using namespace combo;
  typedef tree_type::sibling_iterator sib_it;
  int res = 0;
  tree_type::iterator ty_it = ty.begin();
  if(*ty_it==id::application)
    for(sib_it sib = ty_it.begin();sib != sib_it(ty.last_child(ty_it)); ++sib)
      if(*sib==id::boolean)
	res++;
  return res;
}


  //WARNING : should be action.h but could not do that due to dependency issues
int combo::action_result_arity(const combo::tree_type& ty) {
  using namespace combo;
  typedef tree_type::sibling_iterator sib_it;
  int res = 0;
  tree_type::iterator ty_it = ty.begin();
  if(*ty_it==id::application)
    for(sib_it sib = ty_it.begin();sib != sib_it(ty.last_child(ty_it)); ++sib)
      if(*sib==id::action_result)
	res++;
  return res;
}

std::ostream& operator<<(std::ostream& out, const combo::node_type& n) {
  using namespace combo;
  switch(n) {
  case id::application:
    return out << "->";
  case id::boolean:
    return out << "boolean";
  case id::contin:
    return out << "contin";
  case id::action_result:
    return out << "action_result";
  case id::unknown:
    return out << "unknown";
  default:
    return out << "UNKNOWN_HANDLE";
  }
}

std::istream& operator>>(std::istream& in, combo::node_type& n) {
  using namespace combo;
  std::string str;
  in >> str;
  assert(!str.empty());
  if(str=="->")
    n=id::application;
  else if(str=="boolean")
    n=id::boolean;
  else if(str=="contin")
    n=id::contin;
  else if(str=="action_result")
    n=id::action_result;
  else if(str=="unknown")
    n=id::unknown;
  return in;
}
