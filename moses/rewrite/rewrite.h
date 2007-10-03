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

#ifndef REWRITE_H
#define REWRITE_H

#include "tree.h"
#include "id.h"

namespace rewrite {
  using namespace id;

  template<typename T>
  class Rule {
  public:
    typedef typename tree<T>::sibling_iterator sib_it;
    virtual ~Rule() { }
    virtual void operator()(tree<T>&) const=0;
  };

  template<typename T>
  class RecRule : public Rule<T> {
  public:
    typedef typename tree<T>::sibling_iterator sib_it;
    virtual void visit(tree<T>&,sib_it) const=0;

    virtual void operator()(tree<T>& t) const;
    virtual void operator()(tree<T>& t,sib_it it) const;
  };

  //flattens all associative functions: f(a,f(b,c)) -> f(a,b,c)
  template<typename T>
  class Level : public RecRule<T> {
  public:
    typedef typename Rule<T>::sib_it sib_it;
    void visit(tree<T>& t,sib_it it) const;
  };
  template<typename T>
  void level(tree<T>& t) { Level<T>()(t); }
  template<typename T,typename It>
  void level(tree<T>& t,It it) { Level<T>()(t,it); }

  //merges constants by evaluating sub-expressions when possible
  //if an operator is commutative, op(const,var,const) will become
  //op(op(const,const),var), e.g., +(2,x,1)->+(3,x)
  template<typename T>
  class MergeConstants : public RecRule<T> {
  public:
    typedef typename Rule<T>::sib_it sib_it;
    void visit(tree<T>& t,sib_it it) const;
  };
  template<typename T>
  void merge_constants(tree<T>& t) { MergeConstants<T>()(t); }

  //operations that are commutative-distributive,like a*(b+c) and a&&(b||c),
  //are expanded in hopes of merging
  template<typename T>
  class Expand : public RecRule<T> {
  public:
    typedef typename Rule<T>::sib_it sib_it;
    void visit(tree<T>& t,sib_it it) const;
  };
  template<typename T>
  void expand(tree<T>& t) { Expand<T>()(t); }

  //lexicographically order subtrees of commutative operations
  template<typename T>
  class CanonicalOrder : public RecRule<T> {
  public:
    typedef typename Rule<T>::sib_it sib_it;
    void visit(tree<T>& t,sib_it it) const;
  };
  template<typename T>
  void canonical_order(tree<T>& t) { CanonicalOrder<T>()(t); }
  template<typename T,typename It>
  void canonical_order(tree<T>& t,It it) { CanonicalOrder<T>()(t,it); }

} //~namespace rewrite

/****** IMPLEMENTATION ******/
#include "rewrite_impl.h"

#endif
