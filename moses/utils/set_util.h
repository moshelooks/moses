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

#ifndef SET_UTIL_H
#define SET_UTIL_H

#include <set>
#include <algorithm>
#include <iterator>

template<typename Container,typename It>
void set_difference(Container& dst,It from,It to) {
  Container tmp;
  std::set_difference(dst.begin(),dst.end(),from,to,inserter(tmp,tmp.begin()));
  dst.swap(tmp);
}
template<typename Container>
void difference2(Container& dst,const Container& src) { 
  set_difference(dst,src.begin(),src.end());
}

template<typename Container,typename It>
void set_intersection(Container& dst,It from,It to) {
  Container tmp;
  std::set_intersection(dst.begin(),dst.end(),from,to,
			inserter(tmp,tmp.begin()));
  dst.swap(tmp);
}
template<typename Container>
void intersection2(Container& dst,const Container& src) { 
  set_intersection(dst,src.begin(),src.end());
}

template<typename Container,typename It>
void set_union(Container& dst,It from,It to) {
  Container tmp;
  std::set_union(dst.begin(),dst.end(),from,to,inserter(tmp,tmp.begin()));
  dst.swap(tmp);
}
template<typename Container>
void union2(Container& dst,const Container& src) { 
  set_union(dst,src.begin(),src.end());
}

template<typename It1,typename It2>
bool has_empty_intersection(It1 from1,It1 to1,It2 from2,It2 to2) {
  while (from1!=to1 && from2!=to2)
    if (*from1<*from2)
      ++from1;
    else if (*from2<*from1)
      ++from2;
    else
      return false;
  return true;
}

#endif
