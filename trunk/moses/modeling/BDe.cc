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

#include "BDe.h"
#include <cmath>
#include <iostream>

namespace BDe {
  using namespace std;

  Log2Sum log2Sum;
  gain_t gainOffset=0;

  void Log2Sum::update(int maxSize) {
    if (_log2Sum.size()>=(unsigned int)maxSize)
      return;
    _log2Sum.reserve(maxSize+10);
  
    if (_log2Sum.empty()) {
      _log2Sum.push_back(0);
      _log2Sum.push_back(0);
    }
  
    for (int i=_log2Sum.size();i<maxSize+10;++i)
      _log2Sum.push_back(_log2Sum[i-1]+log((gain_t)(i-1)));
  }

  gain_t evaluateTransfer(int* nFrom,int* nTo,int sumFrom,int sumTo,
			 int* enCats,int* cntCats,int nCats,gain_t oldgain) {
    for (int i=0;i<nCats;++i)
      oldgain-=log2Sum(nFrom[enCats[i]])+log2Sum(nTo[enCats[i]]);
    oldgain+=log2Sum(sumFrom)+log2Sum(sumTo);
    for (int i=0;i<nCats;++i) {
      oldgain+=
	log2Sum(nFrom[enCats[i]]-cntCats[i])+
	log2Sum(nTo[enCats[i]]+cntCats[i]);
      sumFrom-=cntCats[i];
      sumTo+=cntCats[i];
    }
    oldgain-=log2Sum(sumFrom)+log2Sum(sumTo);
    return oldgain;
  }

  void updateTransfer(int* nFrom,int* nTo,int& sumFrom,int& sumTo,
		      int* enCats,int* cntCats,int nCats) {
    for (int i=0;i<nCats;++i) {
      nFrom[enCats[i]]-=cntCats[i];
      nTo[enCats[i]]+=cntCats[i];
      sumFrom-=cntCats[i];
      sumTo+=cntCats[i];
    }
  }    

  /***
  void test() {
    using namespace std;
    using namespace boost;

    vector<int> discVec1(10000),discVec2(10000);
    vector<bool> boolVec1(10000),boolVec2(10000);
    vector<contin_t> continVec1(10000),continVec2(10000);
    generate(discVec1.begin(),discVec1.end(),bind(randint,10));
    generate(discVec2.begin(),discVec2.end(),bind(randint,10));
    generate(boolVec1.begin(),boolVec1.end(),randbool);
    generate(boolVec2.begin(),boolVec2.end(),randbool);
    generate(continVec1.begin(),continVec1.end(),randfloat);
    generate(continVec2.begin(),continVec2.end(),randfloat);
    vector<int> oneside;

    for (int i=0;i<100;++i) {
      cout << BDe::binary_binary(boolVec1.begin(),boolVec1.end(),
				 boolVec2.begin()) 
	   << endl;
      cout << BDe::disc_disc_2way(discVec1.begin(),discVec1.end(),10,
				  discVec2.begin(),10,back_inserter(oneside))
	   << endl;
      cout << BDe::disc_disc_nway(discVec1.begin(),discVec1.end(),10,
				  discVec2.begin(),10) 
	   << endl;
      cout << BDe::contin_disc_2way(continVec1.begin(),continVec1.end(),
				    discVec1.begin(),10,back_inserter(oneside))
	   << endl;
      cout << BDe::contin_disc_nway(continVec1.begin(),continVec1.end(),
				    discVec1.begin(),10) 
	   << endl;
      cout << BDe::disc_contin_2way(discVec1.begin(),discVec1.end(),10,
				    continVec1.begin()).first 
	   << endl;
      cout << BDe::contin_contin_2way(continVec1.begin(),continVec1.end(),
				      continVec2.begin()).first 
	   << endl;
    }
  }
  ***/
} //~namepsace BDe

