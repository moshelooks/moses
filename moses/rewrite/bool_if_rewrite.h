/****
   Copyright 2005-2007, Moshe Looks

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

#ifndef BOOL_IF_REWRITE_H
#define BOOL_IF_REWRITE_H

#include "rewrite.h"

namespace rewrite {

  /***
      if !x y z   -> if x z y

      if x y y    -> y

      if x x y    -> or(x,y)
      if x y !x   -> or(y,!x)

      if x y x    -> and(y,x)
      if x !x y   -> and(!x,y)

      !(if x y z) -> if x !y !z (handled in reduce_nots)

      //not yet supported
      if x and(q,y) and(q,z)    -> and(q,if x y z)
      if x or(q,y) or(q,z)      -> or(q,if x y z)
      or(and(x,y),and(!x,z))    -> if x y z
      and(or(x z) or(y not(x))) -> if x y z
  ***/
  template<typename T>
  class ReduceBoolIfs  : public RecRule<T> {
  public:
    typedef typename Rule<T>::sib_it sib_it;
    void visit(tree<T>& t,sib_it it) const;
  };
  template<typename T>
  void reduce_bool_ifs(tree<T>& t) { ReduceBoolIfs<T>()(t); }
  template<typename T,typename It>
  void reduce_bool_ifs(tree<T>& t,It it) { ReduceBoolIfs<T>()(t,it); }

  template<typename T>
  void ReduceBoolIfs<T>::visit(tree<T>& t,sib_it it) const {
    if (is_bool_if(*it)) {
      //if a b c
      sib_it c=it.begin();
      sib_it a=c++;
      sib_it b=c++;
    
      if (is_not(*a)) { //                  if !x y z   -> if x z y
	t.swap_siblings(b,c);
	a=t.erase(t.flatten(a));
      }
      
      if (t.equal_subtree(b,c)) { //        if x y y    -> y
	*it=*c;
	t.erase(it.begin());
	t.erase(it.begin());
	t.erase(t.flatten(it.begin()));
      } else if (t.equal_subtree(a,b) || // if x x y    -> or(x,y)
		 (is_not(*c) &&          // if x y !x   -> or(y,!x)
		  t.equal_subtree(a,c.begin()))) {
	*it=create_or<T>();
	t.erase(it.begin());
      } else if (t.equal_subtree(a,c) || // if x y x    -> and(y,x)
		 (is_not(*b) &&          // if x !x y   -> and(!x,y)
		  t.equal_subtree(a,b.begin()))) {
	*it=create_and<T>();
	t.erase(it.begin());
      }	  
    }
  }

} //~namespace rewrite

#endif
