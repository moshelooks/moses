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

#ifndef ALGEBRAIC_REWRITE_H
#define ALGEBRAIC_REWRITE_H

#include "rewrite.h"

namespace rewrite {

  //Replaces x-y with  x+(y*(-1))
  template<typename T>
  class RemoveMinus : public RecRule<T> {
  public:
    typedef typename Rule<T>::sib_it sib_it;
    void visit(tree<T>& t,sib_it it) const;
  };
  template<typename T>
  void remove_minus(tree<T>& t) { RemoveMinus<T>()(t); }

  //Replaces x/y with x*y^(-1)
  template<typename T>
  class RemoveDivides : public RecRule<T> {
  public:
    typedef typename Rule<T>::sib_it sib_it;
    void visit(tree<T>& t,sib_it it) const;
  };
  template<typename T>
  void remove_divides(tree<T>& t) { RemoveDivides<T>()(t); }

  //Replaces cos(x) with sin(x+pi/2)
  template<typename T>
  class RemoveCos : public RecRule<T> {
  public:
    typedef typename Rule<T>::sib_it sib_it;
    void visit(tree<T>& t,sib_it it) const;
  };
  template<typename T>
  void remove_cos(tree<T>& t) { RemoveCos<T>()(t); }

  //EvalAlgebraicIdentities
  // 0+a->0,
  // 1*a->0, 0*a->0, 1*a->a
  // a^0->1, a^1->a, 0^a->0, 1^a->1, (a^x)^y->a^(x*y)
  // log(a*b)->log(a)+log(b), log(a^b)->b*log(a)
  template<typename T>
  class EvalAlgebraicIdentities  : public RecRule<T> {
  public:
    typedef typename Rule<T>::sib_it sib_it;
    void visit(tree<T>& t,sib_it it) const;

    void visit_plus(tree<T>& t,sib_it it) const;
    void visit_times(tree<T>& t,sib_it it) const;
    void visit_exp(tree<T>& t,sib_it it) const;
    void visit_log(tree<T>& t,sib_it it) const;
  };
  template<typename T>
  void eval_algebraic_identities(tree<T>& t) { 
    EvalAlgebraicIdentities<T>()(t); 
  }

  //MergeCommons
  //a*x+a*y->a*(x+y) [and rev], a+x*a->(1+x)*a [and rev]
  //a*a->a^2, a*(a^x)->a^(x+1) [and rev]
  //(a^x)*(a^y)->a^(x+y), (x^a)*(y^a)->(x*y)^a
  template<typename T>
  class MergeCommons  : public RecRule<T> {
  public:
    typedef typename Rule<T>::sib_it sib_it;
    void visit(tree<T>& t,sib_it it) const;

    bool explicate_plus(tree<T>& t,sib_it it) const;
    bool explicate_times(tree<T>& t,sib_it it) const;
    void merge_plus(tree<T>& t,sib_it it) const;
    void merge_times(tree<T>& t,sib_it it) const;

  private:
    sib_it sect(tree<T>&,sib_it,sib_it,sib_it, sib_it) const;
  };
  template<typename T>
  void merge_commons(tree<T>& t) { MergeCommons<T>()(t); }

} //~namespace rewrite

/****** IMPLEMENTATION ******/
#include "algebraic_rewrite_impl.h"

#endif
