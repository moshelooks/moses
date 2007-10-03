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

#ifndef SELECTION_H
#define SELECTION_H

#include <ext/functional>
#include <ext/hash_map>
#include <functional>
#include <numeric>
#include <vector>
#include <iterator>
#include <boost/iterator/transform_iterator.hpp>

#include "math_util.h"
#include "func_util.h"

using __gnu_cxx::select1st;
using __gnu_cxx::select2nd;

template<typename It,typename Selector>
It roulette_select(It from,It to,const Selector& sel,int sum=-1) {
  if (sum==-1)
    sum=std::accumulate(from,to,0,compose3(std::plus<int>(),
					   __gnu_cxx::identity<int>(),sel));
  sum=randint(sum);
  do {
    sum-=sel(*from++);
  } while (sum>=0);
  return --from;
}

template<typename It>
It roulette_select(It from,It to,double sum=-1) {
  if (sum==-1)
    sum=std::accumulate(from,to,double(0));
  sum=randfloat()*sum;
  do {
    sum-=*from++;
  } while (sum>=0);
  return --from;
}

template<typename It>
It roulette_select(It from,It to,int sum) {
  return roulette_select(from,to,double(sum));
}

template<typename It,typename Comp>
It tournament_select(It from,It to,int tSize,Comp comp) {
  int dist=std::distance(from,to);
  It res=from+randint(dist);
  for (int i=1;i<tSize;++i) {
    It tmp=from+randint(dist);
    if (comp(*res,*tmp))
      res=tmp;
  }
  return res;
}

template<typename It>
It tournament_select(It from,It to,int tSize) {
  typedef typename std::iterator_traits<It>::value_type T;
  return tournament_select(from,to,tSize,std::less<T>());
}

class Selector : public std::iterator<std::forward_iterator_tag,int> {
public:
  Selector(int n) : _n(n),_v(-1) { }
  bool empty() const { return (_n==0); }
  int operator()() { return next(); }

  int operator*() const { 
    if (_v==-1) 
      _v=next();
    return _v; 
  }
  Selector& operator++() { 
    _v=(_v==-1 ? next() : -1);
    return *this; 
  }
  bool operator==(const Selector& rhs) const { return _n==rhs._n; }
private:
  mutable int _n;
  typedef __gnu_cxx::hash_map<int,int> Map;
  mutable Map _map;
  mutable int _v;

  int next() const;
};

template<typename It,typename T,typename Distance>
It nearest_neighbor(It from,It to,const T& t,const Distance& dist=Distance()) {
  It closest=from;
  typename Distance::result_type closestD;
  for (;from!=to;++from) {
    typename Distance::result_type d=d(t,*from);
    if (d>closestD) {
      d=closestD;
      closest=from;
    }
  }
  return closest;
}

template<typename NodeT>
class NodeSelector {
public:
  typedef NodeT value_type;
  typedef std::pair<NodeT,int> Pair;
  typedef std::vector<Pair> PSeq;
  typedef typename boost::transform_iterator< select1st<Pair>,
					      typename PSeq::const_iterator > 
  node_iterator;

  //annoying waste of precious tokens - is there a more consise way?
  struct arity_iterator {
    int operator*() const { return _i; }
    void operator++() { 
      assert(false);
      /*++_it;
      for (;_it!=_end && *_it==0;++_it)
      ++i;*/
    }
  private:
    int _i;
    std::vector<int>::const_iterator _it;
    const std::vector<int>::const_iterator _end;

    arity_iterator(std::vector<int>::const_iterator it,
		   std::vector<int>::const_iterator end) : 
      _i(0),_it(it),_end(end) 
    {
      for (;_it!=_end && *_it==0;++_it,++_i);
    }
  };
  
  node_iterator begin_with_arity(int arity) const { 
    return node_iterator(_byArity[arity].begin(),select1st<Pair>());
  }
  node_iterator end_with_arity(int arity) const { 
    return node_iterator(_byArity[arity].end(),select1st<Pair>());
  }

  arity_iterator begin_arity() const { 
    return arity_iterator(_aritySums.begin(),_aritySums.end());
  }
  arity_iterator end_arity() const { 
    return arity_iterator(_aritySums.end(),_aritySums.end());
  }
  
  NodeT select_with_arity(int arity) const {
    return roulette_select
      (_byArity[arity].begin(),_byArity[arity].end(),
       select2nd<typename PSeq::value_type>(),_aritySums[arity])->first;
  }
  int count_with_arity(int arity) const { 
    return _byArity[arity].size(); 
  }

  int select_arity(int from) const {
    //could make this slightly faster by caching the partial sums,
    //but who cares?
    return distance(_aritySums.begin(),
		    roulette_select(_aritySums.begin()+from,_aritySums.end()));
  }
  
  void insert(const NodeT& n,int arity,int prob=1) {
    if ((int)_byArity.size()<=arity) {
      _byArity.resize(arity+1);
      _aritySums.resize(arity+1,0);
    }      
    _byArity[arity].push_back(std::make_pair(n,prob));
    _aritySums[arity]+=prob;
  }

  void clear() { _byArity.clear(); _aritySums.clear(); }
private:
  std::vector<PSeq> _byArity;
  std::vector<int> _aritySums;
};
   
#endif
