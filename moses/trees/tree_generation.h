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

#ifndef TREE_GENERATION_H
#define TREE_GENERATION_H

#include "math_util.h"
#include "hash_util.h"
#include "tree.h"
#include "simplify.h"
#include <iterator>
#include <ext/hash_set>

namespace trees {

  template<int MIN_ARITY>
  class Generator {
  public:

    template<typename Selector>
    tree<typename Selector::value_type> operator()(const Selector& s,
						   int dp) const {
      //make sure we don't generate single leaves
      int arity=s.select_arity(1);
      tree<typename Selector::value_type> tr(s.select_with_arity(arity));
      for (int i=0;i<arity;++i)
	build(s,dp,tr,tr.append_child(tr.begin()));
      return tr;
    }

    template<typename Selector,typename T,typename iterator>
    void build(const Selector& s,int dp,tree<T>& tr,iterator it) const {
      if (dp==1) {
	tr.replace(it,s.select_with_arity(0));
      } else {
	int arity(s.select_arity(MIN_ARITY));
	it=tr.replace(it,s.select_with_arity(arity));
	--dp;
	for (int i=0;i<arity;++i)
	  build(s,dp,tr,tr.append_child(it));
      }
    }
  };

  typedef Generator<1> FullGenerator;
  typedef Generator<0> GrowGenerator;

  template<typename It,typename Selector>
  void ramped_half_and_half(It from,It to,const Selector& sel,
			    int minDp,int maxDp) {
    GrowGenerator gg;
    FullGenerator fg;
    float ratio=((float)(maxDp-minDp+1))/(float)distance(from,to);
    for (It tr=from;tr!=to && tr!=(to+1);tr+=2)
      (*tr)=gg(sel,minDp+(int)(distance(from,tr)*ratio));
    for (It tr=from+1;tr!=to && tr!=(to+1);tr+=2)
      (*tr)=fg(sel,minDp+(int)(distance(from,tr)*ratio));
  }

  template<typename It,typename Selector,typename Simplify>
  void ramped_half_and_half_nodup(It from,It to,const Selector& sel,int minDp,
				  int maxDp,const Simplify& simplify) {
    __gnu_cxx::hash_set<typename std::iterator_traits<It>::value_type> trees;
    GrowGenerator gg;
    FullGenerator fg;
    int depth=minDp,n=distance(from,to);
    float ratio=((float)(maxDp-minDp+1))/(float)n;
    for (int i=0;i<n;++i,++from) {
      int nTries=0;
      do {
	(*from)=(i%2) ? 
	  gg(sel,depth) :
	  fg(sel,depth);
	std::cout << *from << " -> ";
	simplify(*from);
	std::cout << *from << std::endl;
	if (++nTries>20) { //magic number lifted from Koza
	  ++depth;
	  nTries=0;
	}
      } while (trees.find(*from)!=trees.end());
      trees.insert(*from);
      depth=std::max(depth,minDp+(int)(i*ratio));
    }
  }

  template<typename It,typename Selector,typename Simplify>
  void ramped_grow_nodup(It from,It to,const Selector& sel,int minDp,
			 int maxDp,const Simplify& simplify) {
    __gnu_cxx::hash_set<typename std::iterator_traits<It>::value_type> trees;
    GrowGenerator gg;
    int depth=minDp,n=distance(from,to);
    float ratio=((float)(maxDp-minDp+1))/(float)n;
    for (int i=0;i<n;++i,++from) {
      int nTries=0;
      do {
	(*from)=gg(sel,depth);
	std::cout << *from << " -> ";
	simplify(*from);
	std::cout << *from << std::endl;
	if (++nTries>20) { //magic number lifted from Koza
	  ++depth;
	  nTries=0;
	}
      } while (trees.find(*from)!=trees.end());
      trees.insert(*from);
      depth=std::max(depth,minDp+(int)(i*ratio));
    }
  }

} //~namespace trees

#endif
