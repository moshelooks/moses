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

#ifndef _MOSES_MOSES_H
#define _MOSES_MOSES_H

#include "reduct/reduct.h"
#include "reduct/meta_rules.h"

#include "util/lru_cache.h"
#include "util/selection.h"
#include "eda/instance_set.h"

#include "moses/using.h"
#include "moses/representation.h"
#include "moses/scoring.h"


using std::cout;
using std::endl;

namespace moses {

  //BScoring = behavioral scoring function (output behaviors)
  template<typename Scoring,typename BScoring,typename Optimization>
  struct metapopulation : public set<behavioral_scored_tree,
				     std::greater<behavioral_scored_tree> > {
    //greater because we are maximizing

    struct parameters {
      parameters() :
	local_cache(false),      //are instance scores cached at the deme level?
	global_cache(false),     //at the global (program) level across demes?
	simplify_all(false),     //are all programs generated simplified?

	selection_max_range(11), //when doing selection of examplars according to
	                         //2^-n, where n is complexity, only examplars with
	                         //p>=2^-selection_max_range will be considered

	scoring_epsilon(0.01)    //scores are considered equal when within epsilon
      { }

      bool local_cache;
      bool global_cache;
      bool simplify_all;

      double selection_max_range;
      double scoring_epsilon;
    };

    metapopulation(const vtree& base,const combo::tree_type& t,
		   const reduct::rule& si,const Scoring& sc,const BScoring& bsc,
		   const Optimization& opt=Optimization(),
		   const parameters& pa=parameters()) :
      type(t),simplify(&si),score(sc),bscore(bsc),optimize(opt),params(pa),
      _n_evals(0),_best_score(worst_possible_score)
    { 
      insert(behavioral_scored_tree
	     (base,behavioral_tree_score(behavioral_score(),
					 tree_score(_best_score.first,0))));
    }

    int n_evals() const { return _n_evals; }
    const tree_score& best_score() const { return _best_score; }
    const std::vector<vtree>& best_trees() const { return _best_trees; }

    const_iterator select_exemplar() const {
      assert(!empty());

      //compute the probs for all candidates
      score_t score=get_score(*begin());
      complexity_t cmin=get_complexity(*begin());
      vector<complexity_t> probs;
      for (const_iterator it=begin();it!=end() && get_score(*it)==score;++it) {
	complexity_t c=get_complexity(*it);
	if (cmin-c>params.selection_max_range)
	  break;
	probs.push_back(c);
      }
      complexity_t sum=0;
      foreach(complexity_t& p,probs) {
	p=(1<<(probs.back()-p));
	sum+=p;
      }
      
      const_iterator exemplar=begin();
      advance(exemplar,distance(probs.begin(),util::roulette_select
				(probs.begin(),probs.end(),sum)));
      return exemplar;
    }

    void expand(int max_evals,const tree_score& max_score) {
      using namespace reduct;

      assert(!empty());

      iterator exemplar=select_exemplar();

      cout << "neeble" << endl;

      //do representation-building and create a deme (initially empty)
      representation rep(*simplify,exemplar->first,type);
      eda::instance_set<tree_score> deme(rep.fields());

      //generate the initial sample to populate the deme

      //remove the examplar and mark it so we won't expand it again
      _visited_exemplars.insert(exemplar->first); 
      erase(exemplar);

      //do some optimization according to the scoring function
      /*_n_evals+=optimize(deme,complexity_based_scorer<Scoring>(score,rep),
	max_evals);*/
      _n_evals+=optimize(deme,count_based_scorer<Scoring>(score,rep,
							  get_complexity(*exemplar)),
			 max_evals);
      cout << "neeble" << endl;
      //add (as potential exemplars for future demes) all unique non-dominated
      //trees in the final deme
      util::hash_map<vtree,behavioral_tree_score,boost::hash<vtree> > candidates;
      foreach(const eda::scored_instance<tree_score>& inst,deme) {
	//if its really bad just skip it
	if (get_score(inst.second)==get_score(worst_possible_score))
	  continue;

	//generate the tree
	rep.transform(inst);
	vtree tr=rep.exemplar();
	
	sequential(clean_reduction(),*simplify)(tr,tr.begin());
	
	//update the set of potential exemplars
	if (_visited_exemplars.find(tr)==_visited_exemplars.end() &&
	    candidates.find(tr)==candidates.end()) {
	  candidates.insert(make_pair(tr,behavioral_tree_score(bscore(tr),
							       inst.second)));

	  //also update the record of the best-seen score & trees
	  if (inst.second>=_best_score) { 
	    if (inst.second>_best_score) {
	      _best_score=inst.second;
	      _best_trees.clear();
	    }
	    _best_trees.push_back(tr);
	  }	    
	}
      }
      cout << "poop" << endl;
      cout << "mm " << candidates.size() << endl;
      cout << "zz " << distance(candidates.begin(),candidates.end()) << endl;

      merge_nondominating(candidates.begin(),candidates.end(),*this);
      cout << "poop" << endl;
      //log some exemplars
      for (const_iterator it=begin();it!=end() && distance(begin(),it)<3;++it)
	cout << "exemplar #" << distance(begin(),it) 
	     << " " << get_tree(*it) << " " 
	     << get_score(*it) << " " 
	     << get_complexity(*it) << endl;
    }

    combo::tree_type type;
    const reduct::rule* simplify;
    Scoring score;
    BScoring bscore; //behavioral score
    Optimization optimize;
    parameters params;
  protected:
    int _n_evals;
    tree_score _best_score;
    std::vector<vtree> _best_trees;

    util::hash_set<vtree,boost::hash<vtree> > _visited_exemplars;
  };
    
  template<typename Scoring,typename Domination,typename Optimization>
  void moses(metapopulation<Scoring,Domination,Optimization>& mp,
	     int max_evals,const tree_score& max_score) {
    while (mp.n_evals()<max_evals) {
      cout << "mp size " << mp.size() << endl;

      //run a generation
      mp.expand(max_evals-mp.n_evals(),max_score);

      //print the generation number and a best solution
      std::cout << "sampled " << mp.n_evals()
		<< " best " << mp.best_score().first << " "
		<< mp.best_trees().front() << std::endl;
      
      if (mp.best_score()>=max_score || mp.empty())
	break;
    }
  }

  template<typename Scoring,typename Domination,typename Optimization>
  void moses(metapopulation<Scoring,Domination,Optimization>& mp,
	     int max_evals,score_t max_score) {
    moses(mp,max_evals,tree_score(max_score,worst_possible_score.second));
  }

} //~namespace moses

#endif
