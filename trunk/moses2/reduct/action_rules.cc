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

#include "reduct/action_rules.h"

namespace reduct {

  typedef vtree::sibling_iterator sib_it;
  typedef vtree::iterator pre_it;


  //action_action_if(A B B) -> and_seq(exec_seq(A) B)
  void reduce_action_action_if::operator()(vtree& tr,vtree::iterator it) const {
    if(*it==id::action_action_if) {
      assert(it.number_of_children()==3);
      pre_it cond = tr.child(it, 0);
      pre_it b1 = tr.child(it, 1);
      pre_it b2 = tr.child(it, 2);
      
      if(tr.equal_subtree(b1,b2)) {
	*it = id::sequential_and;
	tr.erase(b2);
	tr.wrap(cond, id::sequential_exec);
      }
    }
  }

  //T_action_if(action_failure A B) -> B
  //T_action_if(action_success A B) -> A
  //with T in {action, boolean, contin}
  void reduce_const_cond_action_if::operator()
    (vtree& tr,vtree::iterator it) const {
    if(*it==id::action_action_if ||
       *it==id::boolean_action_if ||
       *it==id::contin_action_if) {
      assert(it.number_of_children()==3);
      pre_it cond = tr.child(it, 0);
      pre_it b1 = tr.child(it, 1);
      pre_it b2 = tr.child(it, 2);
      if(*cond==id::action_failure) {
	*it = *b2;
	tr.erase(cond);
	tr.erase(b1);
	tr.erase(tr.flatten(b2));
      }
      else if(*cond==id::action_success) {
	*it = *b1;
	tr.erase(cond);
	tr.erase(b2);
	tr.erase(tr.flatten(b1));
      }
    }
  }
  
  //and_seq(A action_failure B) -> and_seq(A action_failure)
  //and_seq(action_failure) -> action_failure
  //and_seq(A action_success B) -> and_seq(A B)
  //or_seq(A action_failure B) -> or_seq(A B)
  //or_seq(A action_success B) -> or_seq(A action_success)
  //or_seq(action_success) -> action_success
  //exec_seq(A action_failure B) -> exec_seq(A B)
  //exec_seq(A action_success C) -> exec_seq(A B)
  void reduce_const_action_seq::operator()
    (vtree& tr,vtree::iterator it) const {
    if(*it==id::sequential_and) {
      if(it.has_one_child() && *it.begin()==id::action_failure) {
	tr.erase_children(it);
	*it = id::action_failure;
      }
      else {
	bool erase = false;
	for(sib_it sib = it.begin(); sib != it.end();) {
	  if(*sib==id::action_failure) {
	    erase = true;
	    ++sib;
	  }
	  else if(erase || *sib==id::action_success)
	    sib = tr.erase(sib);
	  else ++sib;
	}
      }
    }
    else if(*it==id::sequential_or) {
      if(it.has_one_child() && *it.begin()==id::action_success) {
	tr.erase_children(it);
	*it = id::action_success;
      }
      else {
	bool erase = false;
	for(sib_it sib = it.begin(); sib != it.end();) {
	  if(*sib==id::action_success) {
	    erase = true;
	    ++sib;
	  }
	  else if(erase || *sib==id::action_failure)
	    sib = tr.erase(sib);
	  else ++sib;
	}
      }
    }
    else if(*it==id::sequential_exec) {
      for(sib_it sib = it.begin(); sib != it.end();) {
	if(*sib==id::action_failure || *sib==id::action_success)
	  sib = tr.erase(sib);
	else ++sib;
      }
    }
  }

  //and_seq() -> action_success
  //or_seq() -> action_failure
  //exec_seq() -> action_success
  void reduce_empty_arg_seq::operator()
    (vtree& tr,vtree::iterator it) const {
    if(it.is_childless()) {
      if(*it==id::sequential_and || *it==id::sequential_exec)
	*it = id::action_success;
      else if(*it==id::sequential_or)
	*it = id::action_failure;
    }
  }

} //~namespace reduct
