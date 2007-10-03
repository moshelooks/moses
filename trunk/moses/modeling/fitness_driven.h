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

#ifndef FITNESS_DRIVEN_H
#define FITNESS_DRIVEN_H

#include "make_prototypes.h"
#include "make_empty_model.h"
#include "build_tree.h"
#include "dtree_learn.h"

#include "io_util.h"
#include "selection.h"

#include <boost/iterator/indirect_iterator.hpp>
#include <algorithm>

namespace modeling {

  struct generational_parameters {
    int nToModel;
    int nGen;
    int tSize;
    float noise;
    int windowSize;
  };

  template<typename It,typename FF,typename Simplifier,typename Replacer>
  void run_gen(It from,It to,const FF& f,const Simplifier& simplify,
	       const Replacer& replace,const generational_parameters& params) {
    using namespace alignment;
    using namespace modeling;

    typedef typename iterator_traits<It>::value_type Genotype;
    typedef typename Genotype::value_type T;
    typedef pair<float,Genotype> FP;
    typedef vector<FP> FPSeq;
    typedef typename FPSeq::iterator FPIt;
    typedef vector<Genotype*> PSeq;
    typedef vector<Genotype> Seq;

    //simplify the initial population
    for_each(from,to,simplify);
    
    //compute fitness and store the pop
    FPSeq pop(distance(from,to));
    {
      FPIt it=pop.begin();
      for (It tr=from;tr!=to;++it,++tr)
        *it=make_pair(f(*tr),*tr);
    }
    //transform(from,to,pop.begin(),bind(make_pair<float,Genotype>,
    //			       bind(f,_1),_1));

    for (int gen=0;gen<params.nGen;++gen) {
      //print the generation number and best solution
      FPIt best=max_element(pop.begin(),pop.end(),
			    bind(std::less<float>(),
				 bind(select1st<FP>(),_1),
				 bind(select1st<FP>(),_2)));
      cout << "best " << gen << " " 
      	   << best->first << " " << best->second << endl;
      if (best->first==89)
	exit(0);
      
      //tournament-select nToModel instances to model
      PSeq toModel(params.nToModel);
      for (typename PSeq::iterator it=toModel.begin();it!=toModel.end();++it)
	*it=&(tournament_select(pop.begin(),pop.end(),params.tSize)->second);

      //create prototypes
      vector<ptree> protos;
      make_prototypes(make_indirect_iterator(toModel.begin()),
		      make_indirect_iterator(toModel.end()),
		      params.noise,params.windowSize,back_inserter(protos));

      for (int i=0;i<(int)protos.size();++i) {
	cout << "sz= " << protos[i].size() << " " << count(protos[i].begin())
	     << " " << protos[i].begin()->vlocs.size() << endl;
	cout << protos[i] << endl;
      }

      //build models of the prototypes
      vector<Graph> graphs(protos.size(),Graph(0));
      for_each(protos.begin(),protos.end(),graphs.begin(),
	       constructor<make_empty_model>());
      
      //learn dependencies
      for_each(graphs.begin(),graphs.end(),dtree_learn);

      //sample from the model to generate new instances
      FPSeq newInst(pop.size()/2);
      vector<int> modelFreqs(graphs.size());
      transform(graphs.begin(),graphs.end(),modelFreqs.begin(),
		selectn<Graph>());
      int total=toModel.size(); //sum of freqs for stochastic model selection
      for (FPIt it=newInst.begin();it!=newInst.end();++it) {
	//choose which submodel to select from
	int idx=distance(modelFreqs.begin(),
			 roulette_select(modelFreqs.begin(),modelFreqs.end(),
					 total));
	Graph& g=*(graphs.begin()+idx);
	ptree& proto=*(protos.begin()+idx);
	
	Assignment a(g.size());
	generate_assignment(g.ordering().begin(),g.ordering().end(),g,a);
	cout << "with " << proto << endl
	     << a << endl;

	build_tree(proto,a,it->second);
	
	cout << "built " << it->second << endl;

	simplify(it->second);
	it->first=f(it->second);
      }

      //replace existing instances according to the replacement policy
      replace(newInst.begin(),newInst.end(),pop.begin(),pop.end());
    }
  }

} //~namespace modeling

#endif
