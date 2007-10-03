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

#ifndef _SUBTREE_ORDER_H
#define _SUBTREE_ORDER_H

#include "id.h"
#include "tree.h"

namespace trees {
  
  template<typename T>
  class SubtreeOrder {
  public:
    bool operator()(const tree<T>& t1,const tree<T>& t2) const {
      return (t1.empty() ? true : (t2.empty() ? false : 
				   rec_comp(t1.begin(),t2.begin())>0));
    }

    typedef typename tree<T>::iterator iterator;
    bool operator()(iterator it1,iterator it2) const {
      return (rec_comp(it1,it2)>0);
    }
    int cmp(iterator it1,iterator it2) const {
      return -rec_comp(it1,it2);
    }
  protected:
    int rec_comp(iterator it1,iterator it2) const {
      using namespace id;

      //special case for nots; !a behaves like a except a<!a
      if (is_not(*it1)) {
	if (is_not(*it2))
	  return rec_comp(it1.begin(),it2.begin());
	int res=rec_comp(it1.begin(),it2);
	if (res)
	  return res;
	return -1;
      } else if (is_not(*it2)) {
	int res=rec_comp(it1,it2.begin());
	if (res)
	  return res;
	return 1;
      }

      if (*it1<*it2) 
	return 1;
      else if (*it2<*it1)
	return -1;
      
      int sz1=it1.number_of_children(),sz2=it2.number_of_children();
      if (sz1<sz2)
	return 1;
      else if (sz2<sz1)
	return -1;
      else if (sz1==0)
	return 0;

      for (typename tree<T>::sibling_iterator sib1=it1.end(),
	     sib2=it2.end();sib1!=it1.begin();) {
	int res=rec_comp(--sib1,--sib2);
	if (res)
	  return res;
      }
      
      //all the same..
      return 0;
    }
  };

} //~namespace trees

#endif
