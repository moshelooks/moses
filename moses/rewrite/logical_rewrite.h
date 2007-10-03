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

#ifndef LOGICAL_REWRITE_H
#define LOGICAL_REWRITE_H

#include "rewrite.h"
#include "flat_normal_form.h"

namespace rewrite {

  //EvalLogicalIdentities
  //!true->false, !false->true
  //and(true X)->X, or(true X)->true
  //and(false X)->false, or(false X)->X
  template<typename T>
  class EvalLogicalIdentities  : public RecRule<T> {
  public:
    typedef typename Rule<T>::sib_it sib_it;
    void visit(tree<T>& t,sib_it it) const;
  };
  template<typename T>
  void eval_logical_identities(tree<T>& t) { EvalLogicalIdentities<T>()(t); }
  template<typename T,typename It>
  void eval_logical_identities(tree<T>& t,It it) { 
    EvalLogicalIdentities<T>()(t,it); 
  }

  //ReduceNots
  //!!a->a,!(a&&b)->(!a||!b),!(a||b)->(!a&&!b)
  template<typename T>
  class ReduceNots  : public RecRule<T> {
  public:
    typedef typename Rule<T>::sib_it sib_it;
    void visit(tree<T>& t,sib_it it) const;
  private:
    sib_it merge(tree<T>& t,sib_it prev,sib_it sib) const;
    void shrink(tree<T>& t,sib_it from1,sib_it to1,
	       sib_it from2, sib_it to2) const;
    bool complimentary(const tree<T>& t,sib_it a,sib_it b) const;
  };
  template<typename T>
  void reduce_nots(tree<T>& t) { ReduceNots<T>()(t); }
  template<typename T,typename It>
  void reduce_nots(tree<T>& t,It it) { ReduceNots<T>()(t,it); }
  
  //ReduceOrs
  //heuristic reduction of ORs based on complementary pairs:
  // 1) pairwise implications of conjuncts (X&&a)||((Y&&!a) impl  X&&Y
  // 2) for all pairs of conjuncts, including implications (in X)
  //    if X is a subset of (or equal to) Y, remove Y
  // Also, true||X -> true, false||X -> X
  template<typename T>
  class ReduceOrs  : public RecRule<T> {
  public:
    typedef typename Rule<T>::sib_it sib_it;
    void visit(tree<T>& t,sib_it it) const;
  };
  template<typename T>
  void reduce_ors(tree<T>& t) { ReduceOrs<T>()(t); }
  template<typename T,typename It>
  void reduce_ors(tree<T>& t,It it) { ReduceOrs<T>()(t,it); }

  //ReduceAnds
  //heuristic reduction of ANDs based on complementary pairs:
  // 1) pairwise implications of conjuncts (X||a)&&((Y||!a) impl (X||Y)
  // 2) for all pairs of conjuncts, including implications (in X)
  //    if X is a subset of (or equal to) Y, remove X
  // Also, true&&X -> X, false&&X -> false
  template<typename T>
  class ReduceAnds  : public RecRule<T> {
  public:
    typedef typename Rule<T>::sib_it sib_it;
    void visit(tree<T>& t,sib_it it) const;
  };
  template<typename T>
  void reduce_ands(tree<T>& t) { ReduceAnds<T>()(t); }
  template<typename T,typename It>
  void reduce_ands(tree<T>& t,It it) { ReduceAnds<T>()(t,it); }

} //~namepsace rewrite

#include "logical_rewrite_impl.h"

#endif
