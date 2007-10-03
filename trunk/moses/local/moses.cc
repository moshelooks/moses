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

#include "moses.h"

namespace moses {
  fitness_cache fitness_cache::_instance=fitness_cache();

  int mycomplexity(const vtree::iterator it) {
    if (is_true(*it) || is_false(*it))
      return 0;
    if (is_argument(*it))
      return (hackproblem==JOIN && get_argument_idx(*it)==0 ? 0 : 1);
    if (is_not(*it))
      return mycomplexity(it.begin());
    //assert(is_and(*it) || is_or(*it));
    if (it.begin()==it.end())
      return (hackproblem==ANT ? (!is_progn(*it)) : 0);
    return
      accumulate(make_transform_iterator
		 (make_counting_iterator(it.begin()),mycomplexity),
		 make_transform_iterator
		 (make_counting_iterator(it.end()),mycomplexity),
		 (++it.begin()==it.end() ? 0 : 1));
  }

  int magnitude(const Assignment& ass) {
    int n=0;
    for (Assignment::const_iterator it=ass.begin();it!=ass.end();++it)
      if (const bool* b=boost::get<bool>(&*it))
	n+=(*b);
      else 
	n+=(boost::get<disc_t>(*it)!=0);
    return n;
  }

} //~namespace moses

