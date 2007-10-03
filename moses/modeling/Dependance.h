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

#ifndef DEPENDANCE_H
#define DEPENDANCE_H

#include <ext/hash_set>
#include <ext/hash_map>
#include <functional>
#include <algorithm>

using std::equal_to;
using __gnu_cxx::hash;
//using std::hash;

/**
 * Dependance keeps track of probabalistic dependancies between variables.
 * T is the variable type.
 **/
template<typename T,
	 typename HashFcn=hash<T>,
	 typename EqualKey=equal_to<T> >
class Dependance {
public:
  typedef typename __gnu_cxx::hash_set<T,HashFcn,EqualKey> DepSet;
  typedef typename __gnu_cxx::hash_map<T,DepSet,HashFcn,EqualKey> DepMap;
  //typedef typename std::hash_set<T,HashFcn,EqualKey> DepSet;
  //typedef typename std::hash_map<T,DepSet,HashFcn,EqualKey> DepMap;
  typedef typename DepMap::const_iterator DepMapCIt;
  typedef typename DepMap::iterator DepMapIt;
  typedef typename DepSet::const_iterator DepSetCIt;
  typedef typename DepSet::iterator DepSetIt;

  void addDep(T const& a,T const& b);
  void addDep(const Dependance&);

  bool operator()(T const& a,T const& b) const {
    DepMapCIt it=_above.find(b);
    return (it!=_above.end() && it->second.find(a)!=it->second.end());
  }
  bool above(const T& a,const T& b) const {
    DepMapCIt it=_above.find(b);
    return (it!=_above.end() && it->second.find(a)!=it->second.end());
  }

  void clear() {
    _above.clear();
    _below.clear();
  }

  template<typename It>
  void destructiveOrder(It begin,It end);
 private:
  DepMap _above;
  DepMap _below;

  bool noDep(const T& a) {
    DepMapIt it=_above.find(a);
    if (it==_above.end())
      return true;
    else if (it->second.size()==0) {
      _above.erase(it);
      return true;
    }
    return false;
  }
  void add(const T& a,const T& b) {
    // The following does the same thing as:
    // _above[b].insert(a);
    // _below[a].insert(b);
    // but is over ten times faster (because [] always calls the ctor).
    // Be afraid. Be very afraid...

    typename DepMap::iterator bIt=_above.find(b),aIt=_below.find(a);
    (bIt==_above.end() ? _above.insert(make_pair(b,DepSet())).first : 
     bIt)->second.insert(a);
    (aIt==_below.end() ? _below.insert(make_pair(a,DepSet())).first : 
     aIt)->second.insert(b);
  }
};

template<typename T,typename HashFcn,typename EqualKey>
template<typename It>
void Dependance<T,HashFcn,EqualKey>::destructiveOrder(It begin,It end) {
  while (begin!=end) {
    for (It it=begin;it!=end;++it) {
      if (noDep(*it)) {
	for (DepMapIt it1=_above.begin();it1!=_above.end();++it1)
	  it1->second.erase(*it);
	std::swap(*begin,*it);
	++begin;
	break;
      }
    }
  }
}

template<typename T,typename HashFcn,typename EqualKey>
void Dependance<T,HashFcn,EqualKey>::addDep(const T& a,const T& b) {
  if (operator()(a,b))
    return;
  add(a,b);
  DepSet& aboveA=_above[a];
  DepSet& belowB=_below[b];
  for (DepSetIt it=aboveA.begin();it!=aboveA.end();++it) {
      add(*it,b);
      for (DepSetIt it1=belowB.begin();it1!=belowB.end();++it1)
	add(*it,*it1);
    }
  for (DepSetIt it=belowB.begin();it!=belowB.end();++it)
    add(a,*it);
}

template<typename T,typename HashFcn,typename EqualKey>
void Dependance<T,HashFcn,EqualKey>::addDep(const Dependance& dep) {
  for (DepMapCIt it=dep._above.begin();it!=dep._above.end();++it)
    _above[it->first].insert(it->second.begin(),it->second.end());
  for (DepMapCIt it=dep._below.begin();it!=dep._below.end();++it)
    _below[it->first].insert(it->second.begin(),it->second.end());
}


#endif


