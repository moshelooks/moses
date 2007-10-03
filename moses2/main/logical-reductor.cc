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

using namespace combo;
using namespace std;
using namespace reduct;
using namespace util;

int main() {
  vtree tr;
  while (cin.good()) {
    cin >> tr;
    if (!cin.good())
      break;
    truth_table tt1(tr);
    //cout << "AR" << endl;
    logical_reduce(tr);        
    //cout << "RA" << endl;
    truth_table tt2(tr,integer_log2(tt1.size()));
    cout << tr << endl;
    //cout << "checking tt" << endl;
    if (tt1!=tt2) {
      cout << tt1 << endl << tt2 << endl;
      cerr << "truth-tables don't match!" << endl;
      return 1;
    }
    //cout << "OK" << endl;
  }
  return 0;
}
