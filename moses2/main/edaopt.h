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

#include "eda/scoring_functions.h"
#include "eda/termination.h"
#include "eda/local_structure.h"
#include "eda/replacement.h"
#include "eda/logging.h"
#include "eda/optimize.h"

#include "util/selection.h"

#include <boost/lexical_cast.hpp>

using namespace eda;
using namespace std;
using namespace util;
using namespace boost;

struct optargs {
  optargs(int argc,char** argv,string usage="") { 
    if (argc<5) {
      cerr << "not enough args, usage: " << argv[0] 
	   << " seed length popsize ngens " << usage << endl;
      exit(1);
    }
    try {
      assert(argc>=5);
      srand(lexical_cast<int>(argv[1]));
      length=lexical_cast<int>(argv[2]);
      popsize=lexical_cast<int>(argv[3]);
      n_select=popsize;
      n_generate=popsize/2;
      max_gens=lexical_cast<int>(argv[4]);
    } catch (...) {
      cerr << "invalid args, usage: " << argv[0]
	   << " seed length popsize ngens " << usage << endl;
      exit(1);
    }
  }
  int length;
  int popsize;
  int n_select;
  int n_generate;
  int max_gens;
};
