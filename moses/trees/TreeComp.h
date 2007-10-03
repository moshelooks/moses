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

#ifndef _TREE_COMP_H
#define _TREE_COMP_H

#include "SubtreeOrder.h"

namespace trees {

  template<typename T>
  class TreeComp : public SubtreeOrder<T> {
  public:
    typedef typename tree<T>::iterator iterator;
    TreeComp(const tree<T>& t1) : t(t1) { }
    const tree<T>& t;
    bool operator()(iterator it1,iterator it2) {
      if (it1.node->parent && !id::is_commutative(it1.node->parent->data))
	return (t.index(it1)<t.index(it2));
      return (rec_comp(it1,it2)>0);
    }
  };

} //~namespace trees

#endif
