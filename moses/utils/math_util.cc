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

#include "math_util.h"
#include "selection.h"

using namespace std;

unsigned long int catalan(unsigned int n) { 
  static vector<unsigned long int> table;

  unsigned int sz=table.size();
  if (sz<=n) {
    table.resize(2*(n+1));
    if (sz==0)
      compute_catalans(table.begin(),table.end());
    else
      compute_catalans(table.begin()+sz,table.end(),table[sz-1],sz);
  }
  return table[n];
}

#include <iostream>
using namespace std;

int select_by_catalan(unsigned int n) {
  static vector<double> table;
  static vector<vector<double> > pairwise_sums;

  unsigned int sz=table.size();
  if (sz<=n) {
    table.resize(2*(n+1));
    if (sz==0)
      compute_logs_of_catalans(table.begin(),table.end());
    else
      compute_logs_of_catalans(table.begin()+sz,table.end(),table[sz-1],sz);

    pairwise_sums.resize(table.size());
    for (;sz<pairwise_sums.size();++sz) {
      pairwise_sums[sz].resize(sz);
      for (unsigned int i=0;i<sz;++i) {
	pairwise_sums[sz][i]=pow(double(2),table[i]+table[sz-i-1]-table[sz]);
      }
    }
  }
  
  return (n==0 ? 0 :
	  distance(pairwise_sums[n].begin(),
		   roulette_select(pairwise_sums[n].begin(),
				   pairwise_sums[n].begin(),double(1))));
}

