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

#ifndef TRUNCATION_REPLACEMENT_H
#define TRUNCATION_REPLACEMENT_H

#include <algorithm>
#include <boost/bind.hpp>

namespace fitness {
  using namespace std;
  using namespace boost;

  class TruncationReplacement {
  public:
    template<typename It>
    void operator()(It srcfrom,It srcto,It dstfrom,It dstto) const {
      typedef typename iterator_traits<It>::value_type T;
      partial_sort(dstfrom,dstfrom+distance(srcfrom,srcto),dstto,
		   bind(std::less<float>(),
			bind(select1st<T>(),_1),bind(select1st<T>(),_2)));
      copy(srcfrom,srcto,dstfrom);
    }      
  };
  
} //~namespace fitness

#endif
