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

#ifndef _COMBO_ASSUMPTION_H
#define _COMBO_ASSUMPTION_H

#include "combo/vertex.h"

namespace combo {

  //This file contains all function required to treat assumptions in vtree

  //The assumption set of a vtree are all children (subtrees) of head
  //except the first child designing the "conclusion" tree or main tree.
  //It is assumed that the assumption set is lexicographically ordered
  //this assumption is verified because insert_assumption is the only way
  //to add an element in that set and respect that order.
  
  //insert_assumption
  //inserts a subtree in the assumption set of a vtree
  //Note it is a set so identical subtree are represented only once
  //tr designs the tree where to insert the assumption
  //assum_tr designs the tree where the assumption is taken from
  //assum_it the root of the assumption
  //It is assumed that the tree is not empty
  void insert_assumption(vtree& tr, vtree::iterator assum_it);

  //put in res all interators that have v as content in all expressions
  //if nothing was found return false
  //otherwise return true
  //of all assumptions
  //it is assumed that the tree is not empty
  bool find_vertices_in_assumptions(const vtree& tr, vertex v,
				    std::vector<vtree::iterator>& res);

  //check if all assumptions are the same in tr1 and tr2
  //assumed that tr1 and tr2 are not empty
  bool equal_assumptions(const vtree& tr1, const vtree& tr2);

  //delete all assumptions
  //it is assumed that the tree is not empty
  void delete_all_assumptions(vtree& tr);
  
  bool test();

} //~namespace combo

#endif
