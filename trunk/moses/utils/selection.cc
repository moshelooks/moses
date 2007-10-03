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

#include "selection.h"
#include <cassert>

int Selector::next() const {
  assert(!empty());
  int idx=randint(_n--);

  Map::iterator findIdx=_map.find(idx);
  if (idx==_n) {
    if (findIdx==_map.end())
      return idx;
    int toReturn=findIdx->second;
    _map.erase(findIdx);
    return toReturn;
  } else {
    int toReturn=(findIdx==_map.end()) ? idx : findIdx->second;
    Map::iterator findLast=_map.find(_n);
    if (findLast==_map.end()) {
      _map[idx]=_n;
    } else {
      _map[idx]=findLast->second;
      _map.erase(findLast);
    }
    return toReturn;
  }
}
