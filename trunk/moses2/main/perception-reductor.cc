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

#include "combo/vertex.h"
#include "reduct/reduct.h"
#include "combo/eval.h"
#include "combo/tree_type.h"

#include <iostream>

using namespace std;
using namespace combo;
using namespace reduct;

int main() {

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
    int s = sample_count(ca);

    //produce random inputs
    RndNumTable rnt(s, ca);
    //print rnt, for debugging
    cout << "Rnd matrix :" << endl << rnt;

    try {
      //evalutate tr over rnt and fill mat1
      mixed_action_table mat1(tr, rnt, tr_type);
      //print mat1, for debugging
      cout << "MAT1" << endl << mat1 << endl;

      //print the tree before reduction, for debugging
      cout << "Before : " << tr << endl;

      perception_reduce(tr);

      //evaluate tr over rnt and fill mat2
      mixed_action_table mat2(tr, rnt, tr_type);
      //print mat2, for debugging
      cout << "MAT2" << endl << mat2 << endl;

      cout << "After  : " << tr << endl;
      //if (mat1!=mat2) {
      //cout << mat1 << endl << mat2 << endl;
      //cerr << "mixed-perception-tables don't match!" << endl;
      //return 1;
      //}
    }
    catch(EvalException& e) {
      cout << e.get_message() << " : " << e.get_vertex() << endl;
    }
  }
  return 0;
}
