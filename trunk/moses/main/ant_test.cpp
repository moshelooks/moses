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

#include "scoring.h"
#include "dtree_modeling.h"
#include "simplify.h"
#include "tree_generation.h"
#include "fitness_driven.h"
#include "BDe.h"
#include "ant.h"

#include <boost/lexical_cast.hpp>
#include <iostream>
#include "io_util.h"

#include "TruncationReplacement.h"
#include "RTR.h"

int main(int argc,char** argv) { 
  using namespace boost;
  using namespace std;
  using namespace alignment;
  using namespace rewrite;
  using namespace trees;
  using namespace modeling;
  using namespace fitness;

  while (cin.good()) {
    vtree tr;
    cin >> tr;
    
    if (!cin.good())
      break;
    
    int eaten=0;
    for (int i=1;i<1000;++i) {
      int tmp=Ant<Vertex>(i)(tr);
      if (tmp>eaten) {
	eaten=tmp;
	cout << i << " ate " << eaten << endl;
      }
    }
    cout << "final: " << eaten << endl;
  }
}
