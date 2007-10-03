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

#ifndef _UTIL_LAZY_RANDOM_SELECTOR_H
#define _UTIL_LAZY_RANDOM_SELECTOR_H

#include "util/hash_map.h"

namespace util {

  //a lazy random selector without replacement -
  //lets you select m random integers in [0,n) without replacement each in O(1)
  //and only uses O(m) memory - useful where n is much larger than m
  struct lazy_random_selector {
    lazy_random_selector(int n) : _n(n),_v(-1) { }
    bool empty() const { return (_n==0); }
    int operator()();
  private:
    int _n;
    hash_map<int,int> _map;
    int _v;
  };

} //~namespace util

#endif
