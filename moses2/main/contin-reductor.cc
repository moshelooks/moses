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
#include <iostream>

using namespace std;
using namespace reduct;
using namespace util;

int main() {
  srand(1);

  vtree tr;

  while (cin.good()) {
    cin >> tr;
    if (!cin.good())
      break;

    int a = arity(tr);
    int s = sample_count(a);//return always 5, for the moment

    //produce random inputs
    RndNumTable rnt(s, a);

    try {

      //print rnt, for debugging
      cout << "Rnd matrix :" << endl << rnt;


      //evalutate tr over rnt and fill ct1
      contin_table ct1(tr, rnt);

      //print the tree before reduction, for debugging
      cout << "Before : " << tr << endl;

      contin_reduce(tr);

      //evaluate tr over rnt and fill ct2
      contin_table ct2(tr, rnt);

      cout << "After  : " << tr << endl;
      if (ct1!=ct2) {
	cout << ct1 << endl << ct2 << endl;
	cerr << "contin-tables don't match!" << endl;
	//return 1;
      }
    }
    catch(EvalException& e) {
      cout << e.get_message() << " : " << e.get_vertex() << endl;
    }
  }
  return 0;
}
