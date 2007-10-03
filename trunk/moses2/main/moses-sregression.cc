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

#include "moses/moses.h"
#include "moses/optimization.h"
#include "moses/scoring_functions.h"
#include <boost/lexical_cast.hpp>
#include "reduct/reduct.h"
#include <iostream>

using namespace moses;
using namespace reduct;
using namespace boost;
using namespace std;

int main(int argc,char** argv) { 
  int order,max_evals;
  int arity=1,nsamples=20;
  try {
    if (argc!=4)
      throw "foo";
    srand(lexical_cast<int>(argv[1]));
    order=lexical_cast<int>(argv[2]);
    max_evals=lexical_cast<int>(argv[3]);
  } catch (...) {
    cerr << "usage: " << argv[0] << " seed order maxevals" << endl;
    exit(1);
  }

  tree_type tt(id::application);
  tt.append_children(tt.begin(),id::contin,2);

  RndNumTable rands(nsamples,arity);

  cout << rands << endl;

  metapopulation<contin_score,contin_bscore,univariate_optimization> 
    metapop(vtree(id::plus),
	    tt,contin_reduction(),
	    contin_score(simple_symbolic_regression(order),rands),
	    contin_bscore(simple_symbolic_regression(order),rands));
  
  //had to put namespace moses otherwise gcc-4.1 complains that it is ambiguous
  moses::moses(metapop,max_evals,0);
}
