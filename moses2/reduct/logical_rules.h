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

#ifndef _REDUCT_LOGICAL_RULES_H
#define _REDUCT_LOGICAL_RULES_H

#include "reduct/reduct.h"
#include "reduct/flat_normal_form.h"
#include "combo/tree_type.h"
#include "util/foreach.h"

namespace reduct {
  //ensure that all arguments and or nodes have an and node as their parent.
  //this is important for other normalizations to be able to catch all cases
  struct insert_ands : public crule<insert_ands> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  struct remove_unary_junctors : public crule<remove_unary_junctors> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  struct remove_dangling_junctors : public crule<remove_dangling_junctors> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //and(true X)->X, or(true X)->true
  //and(false X)->false, or(false X)->X
  //or(X)->X
  //and(or X) -> and(X), or(and X) -> or(X)
  struct eval_logical_identities : public crule<eval_logical_identities> { 
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //!!a->a,!(a&&b)->(!a||!b),!(a||b)->(!a&&!b),
  struct reduce_nots : public crule<reduce_nots> { 
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //and(x_1 x_2 x_3 ...)
  //reduce x_1 assuming x_2, x_3 ...
  //reduce x_2 assuming x_1, x_3 ...
  //...
  //and choose the one that shorten the most the expression
  struct reduce_and_assumptions : public crule<reduce_and_assumptions> {
    reduce_and_assumptions(const rule& r) : _reduction(&r) { }
    reduce_and_assumptions() : _reduction(this) { }
    void operator()(vtree& tr,vtree::iterator it) const;
  protected:
    const rule* _reduction;
  };

  //reduce or(x_1 x_2 x_3 ...) by reducing assuming not(and(not(x_1)...))
  //using the reduce_and_assumptions
  struct reduce_or_assumptions : public crule<reduce_or_assumptions> {
    reduce_or_assumptions(const rule& r) : _reduction(&r) { }
    reduce_or_assumptions() : _reduction(this) { }
    void operator()(vtree& tr,vtree::iterator it) const;
  protected:
    const rule* _reduction;
  };

  //heuristic reduction of ORs based on complementary pairs:
  // 1) pairwise implications of conjuncts (X&&a)||((Y&&!a) impl  X&&Y
  // 2) for all pairs of conjuncts, including implications (in X)
  //    if X is a subset of (or equal to) Y, remove Y
  // Also, true||X -> true, false||X -> X
  struct reduce_ors : public crule<reduce_ors> { 
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //heuristic reduction of ANDs based on complementary pairs:
  // 1) pairwise implications of conjuncts (X||a)&&((Y||!a) impl (X||Y)
  // 2) for all pairs of conjuncts, including implications (in X)
  //    if X is a subset of (or equal to) Y, remove X
  // Also, true&&X -> X, false&&X -> false
  struct reduce_ands : public crule<reduce_ands> { 
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //reduction to enf - note that reduce nots and eval_logical_identities and 
  //level should be called first to ensure proper reduction
  struct subtree_to_enf : public crule<subtree_to_enf> {
    void operator()(vtree& tr,vtree::iterator it) const { reduce_to_enf(tr,it); }
  protected:
    struct reduce_to_enf {
      enum Result { Delete,Disconnect,Keep };
      typedef vertex value_type;
      typedef vtree::sibling_iterator sib_it;
      typedef vtree::upwards_iterator up_it;
      typedef util::lexicographic_subtree_order<value_type> Comp;
      typedef std::set<sib_it,Comp> subtree_set;

      vtree& tr;
      Comp comp;

      reduce_to_enf(vtree& tr_,vtree::iterator it) : tr(tr_) { (*this)(it); }

      void operator()(sib_it it);
      bool consistent(const subtree_set& s);
      bool and_cut(sib_it child);
      void or_cut(sib_it current);      
      Result reduce(sib_it current,const subtree_set& dominant,
		    const subtree_set& command);
      Result reduce_and(sib_it current,const subtree_set& dominant,
			const subtree_set& command);
      Result reduce_or(sib_it current,const subtree_set& dominant,
		       const subtree_set& command);
    
      template<typename Out>
      void build_subtree_sets_upwards(up_it up,Out dom_out,Out cmd_out) const {
	for (up_it p=tr.parent(up);
	     (p!=tr.end_upwards() &&
	      vertex_input_type(*p)==id::boolean &&
	      vertex_output_type(*p)==id::boolean);++p) {
	  if (*p==id::logical_and) {
	    for (sib_it sib=p.begin();sib!=p.end();++sib)
	      if (sib!=up)
		*dom_out++=sib;
          } else {
	    assert(*p==id::logical_or);
	    for (sib_it sib=p.begin();sib!=p.end();++sib)
	      if (sib!=up && is_argument(*sib))
		*cmd_out++=sib;
	  }
	  up=p;
	}	    
      }

      template<typename Dst>
      void push_back_negated_arguments(const subtree_set& s,Dst& negated) {
	foreach (sib_it sib,s) {
	  if (is_argument(*sib)) {
	    negated.push_back(vtree(sib));
	    get_argument(*negated.back().begin()).negate();
	  }
	}
      }

      struct tree_eraser { 
	tree_eraser(vtree& t) : tr(t) { }
	vtree& tr;
	template<typename It>
	void operator()(It it) { tr.erase(*it); }
      };
      struct subtree_set_eraser { 
	subtree_set_eraser(subtree_set& s) : se(s) { }
	subtree_set& se;
	template<typename It>
	void operator()(It it) { se.erase(it); }
      };
      struct tree_inserter { 
	tree_inserter(vtree& t,sib_it i) : tr(t),it(i) { }
	vtree& tr;
	sib_it it;
	template<typename It1,typename It2>
	void operator()(It1 it1,It2 it2) { 
	  if (it1->node==0)
	    tr.append_child(it,it2);
	  else
	    tr.insert_subtree(*it1,it2); 
	}
      };
    };
  };

} //~namespace reduct

#endif
