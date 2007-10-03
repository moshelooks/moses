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

#include "hash_util.h"
#include "math_util.h"
#include <iostream>
#include <boost/bind.hpp>

using namespace __gnu_cxx;
using namespace std;
using namespace boost;

template<typename T>
void foo(T n) {
  cout << bind(operator+=,ref(n),_1)(42) << endl;
}

int main() {
  random_access_hash_map<int,int> ma;
  for (int i=0;i<1000;++i)
    ma.insert(make_pair(i,i));
  for (int i=1000;i<2000;++i)
    ma.replace(ma.find(i-1000),make_pair(i,-i));
  for (int i=0;i<1000;++i)
    assert(ma.find(i)==ma.end());
  for (int i=1000;i<2000;++i) {
    assert(ma.find(i)!=ma.end());
    assert(ma.find(i)->second==-i);
  }

  foo(100);
}

  
