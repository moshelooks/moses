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

#ifndef _SIMPLIFY_H
#define _SIMPLIFY_H

#include "rewrite.h"
#include "ant_rewrite.h"
#include "logical_rewrite.h"
#include "bool_if_rewrite.h"

#include "enf.h"

#include "algebraic_rewrite.h"
/*
#include "fuzzy_rewrite.h"

**/

namespace rewrite {
  /**
  template<typename T>
  void simplify(tree<T>& tr) {
    tree<T> tmp;
    
    remove_minus(tr);
    remove_divides(tr);
    remove_cos(tr);

    canonical_order(tr);
    do {
      tmp=tr;
      merge_commons(tr);
      //epsilonize(tr);
      level(tr);
      merge_constants(tr);
      //reduce_almost(tr);
      reduce_less(tr);
      eval_logical_identities(tr);
      reduce_nots(tr);
      reduce_ors(tr);
      reduce_ands(tr);
      eval_algebraic_identities(tr);
      canonical_order(tr);

      if_food_simplify(tr);
    } while (tmp!=tr);
  }

  template<typename T>
  void logical_simplify(tree<T>& tr) {
    tree<T> tmp;
    do {
      tmp=tr;
      level(tr);
      //merge_constants(tr);
      eval_logical_identities(tr);
      reduce_nots(tr);
      reduce_ors(tr);
      reduce_ands(tr);
      expand(tr);
    } while (tmp!=tr);
  }

  template<typename T>
  void fuzzy_simplify(tree<T>& tr) {
    tree<T> tmp;
    do {
      tmp=tr;
      level(tr);
      merge_constants(tr);
      reduce_almost(tr);
      reduce_less(tr);
      eval_logical_identities(tr);
      reduce_nots(tr);
      reduce_ors(tr);
      reduce_ands(tr);
      expand(tr);
      canonical_order(tr);
    } while (tmp!=tr);
  }
  **/
  template<typename T>
  void ant_normalize(tree<T>& tr) {
    using namespace id;
    tree<T> tmp;
    do {
      tmp=tr;
      level(tr);
      ant_simplify(tr);
    } while (tmp!=tr);
    if (!is_progn(*tr.begin()))
      tr.insert_above(tr.begin(),create_progn<T>());
  }

  template<typename T>
  void numeric_normalize(tree<T>& tr) {
    using namespace id;
    tree<T> tmp;

    canonical_order(tr);
    do {
      tmp=tr;
      level(tr);
      merge_constants(tr);
      eval_algebraic_identities(tr);
      merge_commons(tr);
      canonical_order(tr);      
    } while (tmp!=tr);
    //if (is_or(*tr.begin()) || is_variable(*tr.begin()) || is_not(*tr.begin()))
    //tr.insert_above(tr.begin(),create_and<T>());
    /**/
    //reduce_nots(tr);
    canonical_order(tr);
  }

  template<typename T>
  void logical_normalize(tree<T>& tr) {
    using namespace id;
    tree<T> tmp;
    /**/

    for (typename tree<T>::pre_order_iterator it=tr.begin();it!=tr.end();)
      if ((is_and(*it) || is_or(*it)) && it.begin()==it.end())
	it=tr.erase(it);
      else
	++it;
	
    do {
      tmp=tr;

      reduce_tree_to_enf(tr);
      reduce_ands(tr);
      reduce_ors(tr);
      reduce_bool_ifs(tr);
    } while (tmp!=tr);
    for (typename tree<T>::pre_order_iterator it=tr.begin();
	 it!=tr.end();++it)
      if (is_or(*it))
	for (typename tree<T>::sibling_iterator sib=it.begin();
	     sib!=it.end();++sib)
	  if (!is_and(*sib))
	    sib=tr.insert_above(sib,create_and<T>());
    if (is_or(*tr.begin()) || is_variable(*tr.begin()) || is_not(*tr.begin()))
      tr.insert_above(tr.begin(),create_and<T>());
    /**/
    //reduce_nots(tr);
    canonical_order(tr);
  }

  //for convenience
  struct logical_normalizer {
    template<typename T>
    void operator()(tree<T>& tr) const { logical_normalize(tr); }
  };
  //for convenience
  struct numeric_normalizer {
    template<typename T>
    void operator()(tree<T>& tr) const { numeric_normalize(tr); }
  };
  //for convenience
  struct ant_normalizer {
    template<typename T>
    void operator()(tree<T>& tr) const { ant_normalize(tr); }
  };

} //~namespace rewrite

#endif
