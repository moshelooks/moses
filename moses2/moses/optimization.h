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

#ifndef _MOSES_OPTIMIZE_H
#define _MOSES_OPTIMIZE_H

#include "moses/moses.h"
#include "eda/termination.h"
#include "eda/replacement.h"
#include "eda/logging.h"
#include "eda/local_structure.h"
#include "eda/optimize.h"
#include "eda/initialization.h"
#include "util/selection.h"
#include "util/dorepeat.h"

namespace moses {

  struct eda_parameters {
    eda_parameters() :
      term_total(1),         //optimization is teriminated after term_total*n 
      term_improv(1),        //generations, or term_improv*sqrt(N/w) consecutive 
	                     //generations with no improvement (w=windowsize)
      
      pop_size_ratio(20),     //populations are sized at N = popsize_ratio*n^1.05
                             //where n is problem size in info-t bits

      window_size_pop(0.05), //window size for RTR is 
      window_size_len(1),    //min(windowsize_pop*N,windowsize_len*n)

      selection(2),          //if <=1, truncation selection ratio, 
                             //if >1, tournament selection size (should be int)
      selection_ratio(1),    //ratio of population size selected for modeling

      replacement_ratio(0.5),//ratio of population size sampled and integrated


      model_complexity(1),   //model parsimony term log(N)*model_complexity

      terminate_if_gte(0)    //optimization is terminated if best score is >= this
    { }
    
    bool is_tournament_selection() { return selection>1; }
    bool is_truncation_selection() { return selection<=1; }


    //N=p.popsize_ratio*n^1.05
    inline int pop_size(const eda::field_set& fs) {
      return int(ceil((double(pop_size_ratio)*
				pow(information_theoretic_bits(fs),1.05))));
    }

    //min(windowsize_pop*N,windowsize_len*n)
    inline int rtr_window_size(const eda::field_set& fs) {
      return int(ceil(min(window_size_pop*pop_size(fs),
			  window_size_len*information_theoretic_bits(fs))));
    }

    //term_total*n 
    inline int max_gens_total(const eda::field_set& fs) {
      return int(ceil(term_total*information_theoretic_bits(fs)));
    }
    //term_improv*sqrt(N/w) 
    inline int max_gens_improv(const eda::field_set& fs) {
      return int(ceil(term_improv*
			       sqrt(information_theoretic_bits(fs)/
				    rtr_window_size(fs))));
    }

    double term_total;
    double term_improv;

    double pop_size_ratio;
    double window_size_pop;
    double window_size_len;
    double selection;
    double selection_ratio;
    double replacement_ratio;
    double model_complexity;
    double terminate_if_gte;
  };

  
  template<typename Out>
  void generate_initial_sample(const eda::field_set& fs,int n,Out out) {
    dorepeat(n) {
      eda::instance inst(fs.packed_width());

      eda::randomize(fs,inst);

      //bias towards zero
      for (eda::field_set::bit_iterator it=fs.begin_bits(inst);
	   it!=fs.end_bits(inst);++it)
	if (util::randint(2)==0)
	  *it=false;
      for (eda::field_set::disc_iterator it=fs.begin_disc(inst);
	   it!=fs.end_disc(inst);++it)
	if (util::randint(2)==0)
	  *it=0;

      //add it
      *out++=inst;
    }
    //foo42
    //caching not yet integrated
    //note: NOT YET USING RTR
  }

  struct univariate_optimization {
    univariate_optimization(const eda_parameters& p=eda_parameters()) : params(p) {}

    template<typename Scoring>
    int operator()(eda::instance_set<tree_score>& deme,
		   const Scoring& score,int max_evals)
    {
      int pop_size=params.pop_size(deme.fields());
      int max_gens_total=params.max_gens_total(deme.fields());
      int max_gens_improv=params.max_gens_improv(deme.fields());
      int n_select=int(double(pop_size)*params.selection_ratio);
      int n_generate=int(double(pop_size*params.replacement_ratio));

      //adjust parameters based on the maximal # of evaluations allowed
      if (max_evals<pop_size) {
	pop_size=max_evals;
	max_gens_total=0;
      } else {
	max_gens_total=min(max_gens_total,
			   (max_evals-pop_size)/n_generate);
      }

      //create the initial sample
      deme.resize(pop_size);
      generate_initial_sample(deme.fields(),pop_size,deme.begin());

      if (params.is_tournament_selection()) {
	eda::cout_log_best_and_gen logger;
	return eda::optimize
	  (deme,n_select,n_generate,max_gens_total,score,
	   eda::terminate_if_gte_or_no_improv<tree_score>
	   (tree_score(params.terminate_if_gte,worst_possible_score.second),
	    max_gens_improv),
	   util::tournament_selection(int(params.selection)),
	   eda::univariate(),eda::local_structure_probs_learning(),
	   eda::rtr_replacement(deme.fields(),params.rtr_window_size(deme.fields())),
	   logger);
      } else { //truncation selection
	assert(false);
	return 42;
	/*
	return optimize(deme,n_select,n_generate,args.max_gens,score,
			terminate_if_gte_or_no_improv(params.terminate_if_gte,
						      max_gens_improv),
			//truncation selection goes here
			univariate(),local_structure_probs_learning(),
			replace_the_worst(),cout_log_best_and_gen());		
	*/
      }
    }

    eda_parameters params;
  };

} //~namespace moses

#endif

