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

#ifndef _UTIL_SELECTION_H
#define _UTIL_SELECTION_H

#include "util/functional.h"
#include "util/numeric.h"
#include "util/hash_map.h"
#include <iterator>
#include "util/dorepeat.h"

namespace util {
  
  struct tournament_selection {
    tournament_selection(int t_size_) : t_size(t_size_) { }
    int t_size;

    template<typename In,typename Out>
    void operator()(In from,In to,Out dst,int n_select) const {
      typename std::iterator_traits<In>::difference_type d=distance(from,to);
      dorepeat(n_select) {
	In res=from+randint(d);
	dorepeat(t_size-1) {
	  In tmp=from+randint(d);
	  if (*res<*tmp)
	    res=tmp;
	}
	*dst++=*res;
      }
    }
  };

  template<typename It,typename ScoreT>
  It roulette_select(It from,It to,ScoreT sum) {
    sum=ScoreT(double(sum)*util::randdouble());
    do {
      sum-=*from++;
    } while (sum>=0);
    return --from;
  }
  template<typename It>
  It roulette_select(It from,It to) {
    typedef typename std::iterator_traits<It>::value_type score_type;
    return roulette_select(from,to,std::accumulate(from,to,score_type(0)));
  }

  template<typename NodeT>
  class NodeSelector {
  public:
    typedef NodeT value_type;
    typedef std::vector<std::pair<NodeT,int> > PSeq;
  
    NodeT select(int arity) const {
      return roulette_select
	(boost::make_transform_iterator
	 (_byArity[arity].begin(),util::select2nd<typename PSeq::value_type>()),
	 boost::make_transform_iterator
	 (_byArity[arity].end(),util::select2nd<typename PSeq::value_type>()),
	 _aritySums[arity]).base()->first;
    }
    int select_arity(int from) const {
      //could make this slightly faster by caching the partial sums,
      //but who cares?
      return distance(_aritySums.begin(),
        	      roulette_select(_aritySums.begin()+from,_aritySums.end()));
    }

    void add(const NodeT& n,int arity,int prob) {
      if ((int)_byArity.size()<=arity) {
	_byArity.resize(arity+1);
	_aritySums.resize(arity+1,0);
      }
      _byArity[arity].push_back(make_pair(n,prob));
      _aritySums[arity]+=prob;
    }
  private:
    std::vector<PSeq> _byArity;
    std::vector<int> _aritySums;
  };
  
} //~namespace util

#endif
