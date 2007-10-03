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

#ifndef _MOSES_SCORING_H
#define _MOSES_SCORING_H

#include "moses/using.h"
#include "moses/representation.h"
#include "moses/types.h"
#include "reduct/reduct.h"
#include "combo/eval.h"

using namespace std;

namespace moses {

  double information_theoretic_bits(const eda::field_set& fs);

  struct logical_score : public unary_function<vtree,int> {
    template<typename Scoring>
    logical_score(const Scoring& score,int a) : target(score,a),arity(a) { }

    int operator()(const vtree& tr) const;

    combo::truth_table target;
    int arity;
  };
  struct logical_bscore : public unary_function<vtree,behavioral_score> {
    template<typename Scoring>
    logical_bscore(const Scoring& score,int a) : target(score,a),arity(a) { }

    behavioral_score operator()(const vtree& tr) const;
    
    combo::truth_table target;
    int arity;
  };

  struct contin_score : public unary_function<vtree,contin_t> {
    template<typename Scoring>
    contin_score(const Scoring& score,const RndNumTable& r)
      : target(score,r),rands(r) { }

    contin_t operator()(const vtree& tr) const;

    combo::contin_table target;
    RndNumTable rands;
  };
  struct contin_bscore : public unary_function<vtree,behavioral_score> {
    template<typename Scoring>
    contin_bscore(const Scoring& score,const RndNumTable& r)
      : target(score,r),rands(r) { }

    behavioral_score operator()(const vtree& tr) const;

    combo::contin_table target;
    RndNumTable rands;    
  };

  template<typename Scoring>
  struct complexity_based_scorer : public unary_function<eda::instance,tree_score> {
    complexity_based_scorer(const Scoring& s,representation& rep)
      : score(s),_rep(&rep) { }
    
    tree_score operator()(const eda::instance& inst) const {
      using namespace reduct;

      //cout << "top, got " << _rep->fields().stream(inst) << endl;
      _rep->transform(inst);
      vtree tr=_rep->exemplar();

      //reduct::apply_rule(reduct::downwards(reduct::remove_null_vertices()),tr);
      //if simplification of all trees is enabled, we should instead do 
      //apply_rule(downwards(remove_null_vertices()),tr);
      //clean_and_full_reduce(tr);
      clean_and_full_reduce(tr);

      //to maybe speed this up, we can score directly on the exemplar,
      //and have complexity(tr) ignore the null vertices
      
      /*std::cout << "scoring " << tr << " -> " << score(tr) 
	<< " " << complexity(tr.begin()) << std::endl;*/
      
      return tree_score(score(tr),complexity(tr.begin()));
    }

    Scoring score;
  protected:
    representation* _rep;
  };

  template<typename Scoring>
  struct count_based_scorer : public unary_function<eda::instance,tree_score> {
    count_based_scorer(const Scoring& s,representation& rep,int base_count)
      : score(s),_rep(&rep),_base_count(base_count) { }
    
    tree_score operator()(const eda::instance& inst) const {
      cout << "transforming " << _rep->fields().stream(inst) << endl;
      _rep->transform(inst);
      cout << "OK" << endl;
      vtree tr=_rep->exemplar();
      cout << "OK " << tr << endl;
      //reduct::clean_and_full_reduce(tr);
      reduct::clean_reduce(tr);
      reduct::contin_reduce(tr);
      cout << "OKK " << tr << endl;
      return tree_score(score(tr),-int(_rep->fields().count(inst))+_base_count);
    }

    Scoring score;
  protected:
    representation* _rep;
    int _base_count;
   
  };

  tribool dominates(const behavioral_score& x,const behavioral_score& y);

  //this may turn out to be too slow...
  template<typename It,typename Set>
  void merge_nondominating(It from,It to,Set& dst) {
    for (;from!=to;++from) {
      std::cout << "ook " << std::distance(from,to) << std::endl;
      bool nondominated=true;
      for (typename Set::iterator it=dst.begin();it!=dst.end();) {
	tribool dom=dominates(from->second,it->second);
	if (dom) {
	  dst.erase(it++);
	} else if (!dom) {
	  nondominated=false;
	  break;
	} else {
	  ++it;
	}
      }
      if (nondominated)
	dst.insert(*from);
      std::cout << "mook" << std::endl;
    }
  }

} //~namespace moses

#endif
