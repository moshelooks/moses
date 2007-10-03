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
  vtree tr;
  while (cin.good()) {
    cin >> tr;
    if (!cin.good())
      break;
    
    logical_reduce(tr);

    representation rep(logical_reduction(),tr,infer_tree_type(tr));
    
    vtree tmp(rep.exemplar());

    for (int i=0;i<10;++i) { 
      cout << rep.exemplar() << endl;

      eda::instance inst(rep.fields().packed_width());
      for (eda::field_set::disc_iterator it=rep.fields().begin_raw(inst);
	   it!=rep.fields().end_raw(inst);++it)
	it.randomize();	
      
      cout << rep.fields().stream(inst) << endl;
      rep.transform(inst);
      cout << rep.exemplar() << endl;

      rep.clear_exemplar();
      assert(tmp==rep.exemplar());
    }
  }
}
