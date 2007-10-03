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

#include "reduct/reduct.h"
#include "combo/eval.h"
#include "combo/tree_type.h"
#include <iostream>

using namespace std;
using namespace combo;
using namespace reduct;
using namespace util;

int main() {
  srand(1);

  vtree tr;

  while (cin.good()) {
    cin >> tr;
    if (!cin.good())
      break;

    //determine the type of tr
    tree_type tr_type = infer_tree_type_fast(tr);
    cout << "Type : " << tr_type << endl;

    bool ct = check_tree_type(tr, tr_type);

    if(!ct) {
      cout << "Bad type" << endl;
      break;
    }

    int ca = contin_arity(tr_type);
    int s = sample_count(ca);//return always 5, for the moment
                             //TODO, maybe not contin_arity but arity
    //produce random inputs
    RndNumTable rnt(s, ca);
    //print rnt, for debugging
    cout << "Rnd matrix :" << endl << rnt;

    try {
      //evalutate tr over rnt and fill mt1
      mixed_table mt1(tr, rnt, tr_type);
      //print mt1, for debugging
      cout << "MT1" << endl << mt1 << endl;

      //print the tree before reduction, for debugging
      cout << "Before : " << tr << endl;

      mixed_reduce(tr);

      //evaluate tr over rnt and fill mt2
      mixed_table mt2(tr, rnt, tr_type);
      //print mt2, for debugging
      cout << "MT2" << endl << mt2 << endl;

      cout << "After  : " << tr << endl;
      if (mt1!=mt2) {
	cout << mt1 << endl << mt2 << endl;
	cerr << "mixed-tables don't match!" << endl;
	return 1;
      }
    }
    catch(EvalException& e) {
      cout << e.get_message() << " : " << e.get_vertex() << endl;
    }
  }
  return 0;
}
