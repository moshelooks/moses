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

#ifndef _EDA_OPTIMIZE_H
#define _EDA_OPTIMIZE_H

#include "eda/instance_set.h"
#include "util/foreach.h"

using namespace std;

namespace eda {
  
  template
  <typename ScoreT,
   typename ScoringPolicy,
   typename TerminationPolicy,
   typename SelectionPolicy,
   typename StructureLearningPolicy,
   typename ProbsLearningPolicy,
   typename ReplacementPolicy,
   typename LoggingPolicy>
  int optimize(instance_set<ScoreT>& current,
	       int n_select,int n_generate,int max_gens,
	       const ScoringPolicy& score,
	       const TerminationPolicy& termination_criterion,
	       const SelectionPolicy& select,
	       const StructureLearningPolicy& learn_structure,
	       const ProbsLearningPolicy& learn_probs,
	       const ReplacementPolicy& replace,
	       LoggingPolicy& write_log) 
  {
    typedef typename StructureLearningPolicy::model_type model_type;

    cout << "top " << current.size() << endl;
    cout << "top " << distance(current.begin_scores(),
			       current.end_scores()) << endl;

    //compute scores of the initial instance set
    transform(current.begin(),current.end(),current.begin_scores(),score);
    cout << "topd" << endl;

    //main loop
    int gen=0;
    for (;gen<max_gens && !termination_criterion(current.begin(),
						 current.end());++gen) {

      cout << "PPtop" << endl;

      //do logging of instance_set and scores
      write_log(current.begin(),current.end(),current.fields(),gen);
      
      //select promising instances
      std::vector<scored_instance<ScoreT> > promising(n_select);
      select(current.begin(),current.end(),promising.begin(),n_select);

      cout << "PPmid" << endl;

      //initialize the model
      model_type model(current.fields(),promising.begin(),promising.end());

      //update the model
      learn_structure(current.fields(),promising.begin(),promising.end(),model);
      learn_probs(current.fields(),promising.begin(),promising.end(),model);

      //create new instances and integrate them into the current instance set,
      //replacing existing instances

      replace(begin_generator(bind(score_instance<ScoringPolicy>,
				   bind(model),score)),
	      end_generator(bind(score_instance<ScoringPolicy>,
				 bind(model),score),n_generate),
	      current.begin(),current.end());      

      cout << "PPbot" << endl;
    }

    //log the final result
    write_log(current.begin(),current.end(),current.fields(),gen);

    //return # of evaluations actually performed
    return current.size()+gen*n_generate;
  }

} //~namespace eda

#endif
