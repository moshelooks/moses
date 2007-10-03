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

#ifndef _COMBO_ACTION_EVAL_H
#define _COMBO_ACTION_EVAL_H

#include "combo/vertex.h"
#include "util/exception.h"
#include "combo/using.h"
#include "combo/tree_type.h"
#include "util/tree.h"


namespace combo {
  inline vertex& binding(int idx) {
    static util::hash_map<int,vertex> map;
    return map[idx];
  }

  template<typename It>
  vertex action_eval_throws(It it) throw(EvalException) {
    typedef typename It::sibling_iterator sib_it;
    //argument
    //WARNING : we assume the argument is necessarily an action and therefor
    //never has a negative index
    if(is_argument(*it)) {
      int idx=get_argument(*it).idx;
      //assumption : when idx is negative the argument is necessary boolean
      assert(idx>0);
      return binding(idx);
    }
    //action sequence
    if(*it==id::sequential_and) {
      for(sib_it sib = it.begin(); sib != it.end(); ++sib) {
	if(action_eval_throws(sib) != id::action_success)
	  return id::action_failure;
      }
      return id::action_success;
    }
    else if(*it==id::sequential_or) {
      for(sib_it sib = it.begin(); sib != it.end(); ++sib) {
	if(action_eval_throws(sib) == id::action_success)
	  return id::action_success;
      }
      return id::action_failure;
    }
    else if(*it==id::sequential_exec) {
      for(sib_it sib = it.begin(); sib != it.end(); ++sib)
	action_eval_throws(sib);
      return id::action_success;
    }
    //action_if
    else if(*it==id::boolean_action_if) {
      assert(it.number_of_children()==3);
      sib_it sib = it.begin();
      vertex vcond = action_eval_throws(sib);
      ++sib;
      if(vcond==id::action_success) {
	//TODO : check that b1 and b2 have type boolean
	return eval_throws(sib);
      }
      else {
	++sib;
	return eval_throws(sib);
      }
    }
    else if(*it==id::contin_action_if) {
      assert(it.number_of_children()==3);
      sib_it sib = it.begin();
      vertex vcond = action_eval_throws(sib);
      ++sib;
      if(vcond==id::action_success) {
	//TODO : check that b1 and b2 have type contin
	return eval_throws(sib);
      }
      else {
	++sib;
	return eval_throws(sib);
      }
    }
    else if(*it==id::action_action_if) {
      assert(it.number_of_children()==3);
      sib_it sib = it.begin();
      vertex vcond = action_eval_throws(sib);
      ++sib;
      if(vcond==id::action_success) {
	//TODO : check that b1 and b2 have type action
	return action_eval_throws(sib);
      }
      else {
	++sib;
	return action_eval_throws(sib);
      }
    }
    //elementary action
    else if(*it==id::dummy_action) {
      std::cout << "Performing dummy_action" << std::endl;
      return id::action_success;
    }
    else {
      return *it;
    }
  }

}

#endif
