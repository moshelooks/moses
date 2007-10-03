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

#include "combo/complexity.h"
#include "combo/using.h"

namespace combo {

  // for a Boolean formula, the complexity is the # of literals

  complexity_t complexity(vtree::iterator it) {
    if (*it==id::logical_true || *it==id::logical_false || *it==id::null_vertex)
      return 0;
    if (is_argument(*it))
      return -1;
    if (*it==id::logical_not)
      return complexity(it.begin());
    assert(*it==id::logical_and || *it==id::logical_or);

    int c=0;
    for (vtree::sibling_iterator sib=it.begin();sib!=it.end();++sib)
      c+=complexity(sib);
    return c;
  }

} //~namespace combo
