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

#include <fstream>
#include "simplify.h"
#include "tree_generation.h"
#include "semantic_sampler.h"

#include "BDe.h"

#include "boolean.h"
#include "ant.h"

#include <boost/lexical_cast.hpp>
#include <iostream>
#include "io_util.h"

#include "scoring.h"

#include "enf.h"
#include "id.h"

#include "build_tree.h"

#include "lmodeling.h"

#include "moses.h"

#include "join.h"

using namespace boost;
using namespace std;
using namespace alignment;
using namespace rewrite;
using namespace trees;
using namespace modeling;
using namespace fitness;

using namespace local;
using namespace moses;
using namespace id;

int main(int argc,char** argv) { 
  /************

  vector<Assignment> ass(4,Assignment(3));
  ass[0][0]=0;
  ass[0][1]=0;
  ass[0][2]=0;


  ass[1][0]=0;
  ass[1][1]=0;
  ass[1][2]=0;


  ass[2][0]=42;
  ass[2][1]=42;
  ass[2][2]=42;

  ass[3][0]=2;
  ass[3][1]=2;
  ass[3][2]=2;

  vtree vtr(create_and<Vertex>());
  ktree ktr=ktree(kseq());
  ktr.append_child(ktr.begin(),kseq
		   (1,Removal(vtr.begin(),
			      vtr.append_child(vtr.begin(),
					       create_argument<Vertex>(1)))));
  ktr.append_child(ktr.begin(),kseq
		   (1,Removal(vtr.begin(),
			      vtr.append_child(vtr.begin(),
					       create_argument<Vertex>(2)))));
  ktr.append_child(ktr.begin(),kseq
		   (1,Removal(vtr.begin(),
			      vtr.append_child(vtr.begin(),
					       create_argument<Vertex>(3)))));

  Graph g(4);
  make_empty_model(ktr,ass.begin(),ass.end(),g);

  split_on_dtree(Split(false,1),g[1],get_tree(g[2]).begin(),g[2]);
  
  Assignment foo(3);
  for (int i=0;i<100;++i) {
    generate_assignment(make_counting_iterator(0),make_counting_iterator(3),
			g,foo);
    cout << foo << endl;
  }
******/
  int ba=0,nGen=0,k=0,popSize=0;
  float tMin=0;
  NodeSelector<Vertex> sel;
  CaseBasedBoolean<Vertex> bc;
  int mpar=0;
  try {
    if (argc==7 || argc==8) {
      srand(lexical_cast<int>(argv[1]));
    
      ba=lexical_cast<int>(argv[2]);
      if (argc==7) {
	local::hackproblem=local::BOOLEAN;
	sel.insert(create_and<Vertex>(),2);
	sel.insert(create_or<Vertex>(),2);
	sel.insert(create_not<Vertex>(),1);
	for (int i=1;i<=ba;++i)
	  sel.insert(create_argument<Vertex>(i),0);
      } else {	
	try {
	  mpar=lexical_cast<int>(argv[7]);
	} catch (...) {
	  mpar=-1;
	  if (argv[7]==string("deceptive") ||
	      argv[7]==string("order")) {
	    local::joinarity=ba;
	    local::hackproblem=local::JOIN;
	    modeling::hack_arity=ba*2+1;
	    sel.insert(create_progn<Vertex>(),2);
	    for (int i=1;i<=ba;++i) {
	      sel.insert(create_argument<Vertex>(i),0);
	      sel.insert(create_argument<Vertex>(-i),0);
	    }
	    /**	  } else if (argv[7]==string("bf")) {
	    local::bfarity=ba;
	    local::hackproblem=local::BINARY_FUNCTIONS;
	    modeling::hack_arity=ba*2+1;
	    sel.insert(create_progn<Vertex>(),2);
	    for (int i=1;i<=ba;++i) {
	      sel.insert(create_argument<Vertex>(i),0);
	      sel.insert(create_argument<Vertex>(-i),0);
	      }**/
	  } else if (string(argv[7])=="ant") {
	    local::hackproblem=local::ANT;
	    sel.insert(create_if_food<Vertex>(),0);
	    sel.insert(create_move<Vertex>(),0);
	    sel.insert(create_right<Vertex>(),0);
	    sel.insert(create_left<Vertex>(),0);
	  } else if (string(argv[7])=="multiplex") { //multiplex
	    local::hackproblem=local::BOOLEAN;
	    sel.insert(create_and<Vertex>(),2);
	    sel.insert(create_or<Vertex>(),2);
	    sel.insert(create_not<Vertex>(),1);
	    for (int i=1;i<=(ba+(1<<ba));++i)
	      sel.insert(create_argument<Vertex>(i),0);
	  } else if (string(argv[7])=="cfs") { //multiplex
	    local::hackproblem=local::BOOLEAN;
	    ifstream in("cfs");
	    bc=CaseBasedBoolean<Vertex>(in);
	    ba=bc.arity();
	    sel.insert(create_and<Vertex>(),2);
	    sel.insert(create_or<Vertex>(),2);
	    sel.insert(create_not<Vertex>(),1);
	    for (int i=1;i<=ba;++i)
	      sel.insert(create_argument<Vertex>(i),0);
	  } else {
	    cerr << "unrecognized problem " << argv[7] << endl;
	    exit(1);
	  }
	}
	if (mpar!=-1) {
	  local::hackproblem=local::BOOLEAN; //hierarchical mplex-par
	  sel.insert(create_and<Vertex>(),2);
	  sel.insert(create_or<Vertex>(),2);
	  sel.insert(create_not<Vertex>(),1);
	  for (int i=1;i<=mpar*(ba+(1<<ba));++i)
	    sel.insert(create_argument<Vertex>(i),0);
	}
      }

      nGen=lexical_cast<int>(argv[3]);
      k=lexical_cast<int>(argv[4]);
      popSize=lexical_cast<int>(argv[5]);
      tMin=lexical_cast<float>(argv[6]);
    } else if (argc==4) {
      int arity=lexical_cast<int>(argv[1]);
      int n=lexical_cast<int>(argv[2]);
      int m=lexical_cast<int>(argv[3]);
      
      local::hackproblem=local::BOOLEAN;
      sel.clear();
      sel.insert(create_and<Vertex>(),2);
      sel.insert(create_or<Vertex>(),2);
      sel.insert(create_not<Vertex>(),1);
      for (int i=1;i<=arity;++i)
	sel.insert(create_argument<Vertex>(i),0);
	
      semantic_sampler<Vertex,logical_normalizer> ss(sel);

      ss.uniform_sample(n,m);

      typedef semantic_sampler<Vertex,logical_normalizer>::TreeSeq TreeSeq;
      for (int i=0;i<=n;++i)
	for (TreeSeq::const_iterator it=ss.trees(i).begin();
	     it!=ss.trees(i).end();++it)
	  cout << complexity(*it) << " " << *it << endl;
      exit(0);
    } else if (argc==1) {
      while (cin.good()) {
	vtree vtr;
	cin >> vtr;
	if (!cin.good())
	  exit(0);

	numeric_normalize(vtr);
	logical_normalize(vtr);
	cout << endl << vtr << endl;

	int arity=2;
	for (vtree::iterator it=vtr.begin();it!=vtr.end();++it)
	  if (is_argument(*it))
	    arity=max(arity,get_argument_idx(*it));
	
	cout << TruthTable(vtr,arity) << endl
	     << TruthTable(EvenParity(),arity) << endl << endl;

	local::hackproblem=local::BOOLEAN;
	sel.clear();
	sel.insert(create_and<Vertex>(),2);
	sel.insert(create_or<Vertex>(),2);
	sel.insert(create_not<Vertex>(),1);
	for (int i=1;i<=arity;++i)
	  sel.insert(create_argument<Vertex>(i),0);
	vector<vtree> perm;
	ktree ktr;
	enumerate_minimal_trees(1,sel,logical_normalizer(),
				back_inserter(perm));
	
	/*copy(perm.begin(),perm.end(),ostream_iterator<vtree>(cout," "));
	cout << endl;*/

	/**/
	neighborhood<logical_normalizer> ni(1,perm,sel);
	ni.exemplar(vtr);
	cout << "exemplar: " << ni.exemplar() << endl;
        semantic_sampler<Vertex,logical_normalizer> ss(sel);
        ni.make_knobs(ss);
	cout << ni.knobs() << endl;
	/**/
	//make_ktree(logical_normalizer(),perm.begin(),perm.end(),vtr,ktr);
      }
    } else if (argc==2) {
      if (string(argv[1])=="ant") {
	local::hackproblem=local::ANT;
	sel.insert(create_if_food<Vertex>(),0);
	sel.insert(create_move<Vertex>(),0);
	sel.insert(create_right<Vertex>(),0);
	sel.insert(create_left<Vertex>(),0);

	/**
	semantic_sampler<Vertex,AntSimplify<Vertex> > ss(sel);
	ss.uniform_sample(10,10);
	for (int i=0;i<10;++i)
	  for (int j=0;j<ss.trees(i).size();++j)
	    cout << ss.trees(i)[j] << endl;
	**/
	    
	while (cin.good()) {
	  vtree vtr;
	  cin >> vtr;
	  if (!cin.good())
	    exit(0);

	  float f1=Ant<Vertex>()(vtr);

	  ant_normalize(vtr);
	  cout << endl << vtr << endl;

	  float f2=Ant<Vertex>()(vtr);
	  
	  if (f1!=f2) {
	    cout << f1 << "!=" << f2 << endl;
	    exit(1);
	  }
	  cout << f2 << endl << endl;

	vector<vtree> perm;
	  neighborhood_params<ant_normalizer> ni(k,perm,sel);
	  neighborhood<ant_normalizer> nu(ni);
	  nu.exemplar(vtr);
	  semantic_sampler<Vertex,ant_normalizer> ss(sel);
	  nu.make_knobs(ss);
	}
	exit(0);
      }
      k=lexical_cast<int>(argv[1]);
      while (cin.good()) {
	vtree vtr;
	cin >> vtr;
	if (!cin.good())
	  exit(0);

	logical_normalize(vtr);
	cout << endl << vtr << endl;
	
	int arity=2;
	for (vtree::iterator it=vtr.begin();it!=vtr.end();++it)
	  if (is_argument(*it))
	    arity=max(arity,get_argument_idx(*it));
	
	TruthTable tt1=TruthTable(vtr,arity);

	local::hackproblem=local::BOOLEAN;
	sel.clear();
	sel.insert(create_and<Vertex>(),2);
	sel.insert(create_or<Vertex>(),2);
	sel.insert(create_not<Vertex>(),1);
	for (int i=1;i<=arity;++i)
	  sel.insert(create_argument<Vertex>(i),0);
	
	vector<vtree> perm;
	ktree ktr;
	enumerate_minimal_trees(k,sel,logical_normalizer(),
				back_inserter(perm));
	cout << "permutations: ";
	copy(perm.begin(),perm.end(),ostream_iterator<vtree>(cout," "));
	cout << endl;

	neighborhood<logical_normalizer> ni(k,perm,sel);
	ni.exemplar(vtr);

	cout << ni.exemplar() << endl;
	
	TruthTable tt2=TruthTable(ni.exemplar(),arity);
	if (!(tt1==tt2)) {
	  cerr << "ttmismatch"
	       << endl << tt1 
	       << endl << tt2 << endl;
	  exit(1);
	}
	
	/**ni.make_knobs();
	   cout << endl;**/
      }
    } else if (argc==3 && string(argv[1])=="cfs") {
      local::hackproblem=local::BOOLEAN;
      ifstream in(argv[2]);
      bc=CaseBasedBoolean<Vertex>(in);
      ba=bc.arity();
      sel.insert(create_and<Vertex>(),2);
      sel.insert(create_or<Vertex>(),2);
      sel.insert(create_not<Vertex>(),1);
      for (int i=1;i<=ba;++i)
	sel.insert(create_argument<Vertex>(i),0);
      sel.insert(create_if_food<Vertex>(),0);
      sel.insert(create_move<Vertex>(),0);
      sel.insert(create_right<Vertex>(),0);
      sel.insert(create_left<Vertex>(),0);
      
      while (cin.good()) {
	vtree vtr;
	cin >> vtr;
	if (!cin.good())
	  exit(0);
	
	logical_normalize(vtr);
	cout << endl << vtr << endl;
	cout << bc(vtr) << " " << bc(vtr.begin()) << endl << endl;
      }
      exit(0);
    } else {
      throw string("wrong # of args");
    }
  } catch (...) {
    cerr << "usages: " << endl
	 << "    " << argv[0] << " randseed arity n_gen k pop_size" << endl
	 << "    " << argv[0] << endl;
    exit(1);
  }

  int window_size=8;
  float scoring_noise=0;

  //set gain offset for modeling
  BDe::gainOffset=(log2((gain_t)popSize)/2.0);

  //compute the set of permutors
  vector<vtree> perm;
  Scorer scorer(scoring_noise);
  
  try {
    if (argc==7) {
      EvenParity ep;
      Boolean<Vertex,EvenParity> boo(ep,ba);
      enumerate_minimal_trees(k,sel,logical_normalizer(),
			      back_inserter(perm));
      neighborhood_params<logical_normalizer> ni(k,perm,sel);
      config<Boolean<Vertex,EvenParity>,Scorer> c(boo,scorer,window_size);
      //moses::moses(ni,c,popSize,nGen);
      moses::megamoses(ni,c,popSize,nGen,tMin);
    } else if (mpar==-1) {
      if (argv[7]==string("deceptive")) {
	Deceptive dec(3,0.25);
	Join<Vertex,Deceptive> joo(dec,ba);
	/*enumerate_minimal_trees(k,sel,identity<tree<Vertex> >(),
	  back_inserter(perm));*/
	neighborhood_params<join_clean> ni(k,perm,sel);
	config<Join<Vertex,Deceptive>,Scorer> c(joo,scorer,window_size);

	vtree start(create_argument<Vertex>(0));
	int depth=int(log2(log2(ba)+1)+0.5);
	//int depth=2;
	make_join_base(start,start.begin(),depth);
	cout << "start is " << start << endl;

	moses::megamoses(ni,c,popSize,nGen,tMin,ba*4-0.99,start);
      } else if (argv[7]==string("order")) {
	Order ord;
	Join<Vertex,Order> joo(ord,ba);
	/*enumerate_minimal_trees(k,sel,identity<tree<Vertex> >(),
	  back_inserter(perm));*/
	neighborhood_params<join_clean> ni(k,perm,sel);
	config<Join<Vertex,Order>,Scorer> c(joo,scorer,window_size);

	vtree start(create_progn<Vertex>());
	int depth=2;
	make_join_base(start,start.begin(),depth);
	cout << "start is " << start << endl;

	moses::megamoses(ni,c,popSize,nGen,tMin,ba-0.99,start);
      } else if (argv[7]==string("ant")) {
	Ant<Vertex> ant;
	neighborhood_params<ant_normalizer> ni(k,perm,sel);
	config<Ant<Vertex>,Scorer> c(ant,scorer,window_size);

	vtree start(create_progn<Vertex>());
	cout << "start is " << start << endl;

	moses::megamoses(ni,c,popSize,nGen,tMin,88.01,start);
      } else if (argv[7]==string("cfs")) {
	enumerate_minimal_trees(k,sel,logical_normalizer(),
				back_inserter(perm));
	neighborhood_params<logical_normalizer> ni(k,perm,sel);
	config<CaseBasedBoolean<Vertex>,Scorer> c(bc,scorer,window_size);
	
	moses::megamoses(ni,c,popSize,nGen,tMin);
      } else {
	assert(argv[7]==string("multiplex"));
	Multiplex mp(ba);
	Boolean<Vertex,Multiplex> boo(mp,ba+(1<<ba));
	enumerate_minimal_trees(k,sel,logical_normalizer(),
				back_inserter(perm));
	neighborhood_params<logical_normalizer> ni(k,perm,sel);
	config<Boolean<Vertex,Multiplex>,Scorer> c(boo,scorer,window_size);
	
	moses::megamoses(ni,c,popSize,nGen,tMin);
      }
      exit(0);
    } else {
      cout << "multiplexing parities, total arity " << mpar*(ba+(1<<ba)) << endl;
      MultiplexedParities mpp(mpar,ba);
      Boolean<Vertex,MultiplexedParities> boo(mpp,mpar*(ba+(1<<ba)));
      enumerate_minimal_trees(k,sel,logical_normalizer(),
			      back_inserter(perm));
      neighborhood_params<logical_normalizer> ni(k,perm,sel);
      config<Boolean<Vertex,MultiplexedParities>,Scorer> c
	(boo,scorer,window_size);
      moses::megamoses(ni,c,popSize,nGen,tMin);
      exit(0);      
    }
  } catch (bad_alloc) {
    cout << "bad_alloc" << endl;
    cerr << "bad_alloc" << endl;
  } catch (...) {
    cerr << "weird exception" << endl;
  }

  /*population<logical_normalizer,Boolean<Vertex,EvenParity>,Scorer>
    pop(ni,boo,scorer,popSize,window_size);*/

  /*
  moses::params mp;
  mp.selector=boolSelector;
  mp.k=n;
  mp.pop_size=popSize;
  mp.n_generations=sz;
  mp.n_to_model=popSize;
  mp.t_size=2;

  moses::moses(boo,logical_normalizer(),
  make_rtr(window_size,Scorer(scoring_noise)),mp);*/

  /*cout << "pdq" << endl;
  //generate the initial sample
  neighborhood<logical_normalizer> ni(n,boolSelector);

  cout << "neighborhood size: " << ni.knobs().size()-1 << endl;

  vector<Assignment> asses(popSize);
  sample_assignments(ni,bind(local::generate_assignment,_1,0.1,_2),popSize,
  asses.begin());*/

  /*for (int j=0;j<popSize;++j) {
    vtree vtr;
    instantiate(asses[j],ni.knobs(),ni.exemplar(),vtr);
    cout << vtr << " -> ";
    logical_normalize(vtr);
    cout << vtr << endl << complexity(vtr.begin()) << endl;
  }
  */
}
      

    /*
    Graph g(popSize);
    //now, build decision trees based on the knobs
    make_model(knobs.begin(),knobs.end(),g);
    */

    /*tree<Vertex> res=local_search(boolSelector,ss.trees(sz)[i]);
    cout << "to " << res
    << "(" << boo(ss.trees(sz)[i]) << ")" << endl;*/

