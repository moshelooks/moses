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

#include "lazy_random_selector.h"
#include "util/numeric.h"
#include <cassert>

namespace util {

  int lazy_random_selector::operator()() {
    assert(!empty());
    int idx=randint(_n--);

    hash_map<int,int>::iterator it=_map.find(idx);
    if (idx==_n) {
      if (it!=_map.end()) {
	idx=it->second;
	_map.erase(it);
      }
      return idx;
    }
    int res=(it==_map.end()) ? idx : it->second;
    hash_map<int,int>::iterator last=_map.find(_n);
    if (last==_map.end()) {
      _map.insert(std::make_pair(idx,_n));
    } else {
      _map.insert(std::make_pair(idx,last->second));
      _map.erase(last);
    }
    return res;
  }

} //~namespace util
