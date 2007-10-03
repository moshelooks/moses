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
  int arity,max_evals;
  try {
    if (argc!=4)
      throw "foo";
    srand(lexical_cast<int>(argv[1]));
    arity=lexical_cast<int>(argv[2]);
    max_evals=lexical_cast<int>(argv[3]);
  } catch (...) {
    cerr << "usage: " << argv[0] << " seed arity maxevals" << endl;
    exit(1);
  }

  tree_type tt(id::application);
  tt.append_children(tt.begin(),id::boolean,arity+1);

  even_parity scorer;
  //disjunction scorer;

  metapopulation<logical_score,logical_bscore,univariate_optimization> 
    metapop(vtree(id::logical_and),tt,logical_reduction(),
	    logical_score(scorer,arity),
	    logical_bscore(scorer,arity));
  
  cout << "build metapop" << endl;

  //had to put namespace moses otherwise gcc-4.1 complains that it is ambiguous
  moses::moses(metapop,max_evals,0);
}
