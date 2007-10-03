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

#ifndef _UTIL_LRU_CACHE_H
#define _UTIL_LRU_CACHE_H

#include <list>
#include <functional>
#include "util/hashing.h"
#include "util/hash_map.h"

namespace util {

  template<typename F,
	   typename Hash=boost::hash<typename F::argument_type>,
	   typename Equals=std::equal_to<typename F::argument_type> >
  struct lru_cache {
    typedef typename F::argument_type argument_type;
    typedef typename F::result_type result_type;
    typedef typename std::list<argument_type> list;
    typedef typename list::iterator list_iter;
    typedef util::hash_map<list_iter,result_type,
			   util::deref_hash<list_iter,Hash>,
			   util::deref_equals<list_iter,Equals> > map;
    typedef typename map::iterator map_iter;
    typedef typename map::size_type size_type;
  
    lru_cache(size_type n,const F& f=F()) : _n(n),_map(n+1),_f(f) { }

    bool full() const { return _map.size()==_n; }
    bool empty() const { return _map.empty(); }
    
    result_type operator()(const argument_type& x) const {
      if (empty()) {
	if (full()) //so a size-0 cache never needs hashing
	  return _f(x);
	_lru.push_front(x);
	map_iter it=_map.insert(make_pair(_lru.begin(),_f(x))).first;
	return it->second;
      }

      //search for it
      _lru.push_front(x);
      map_iter it=_map.find(_lru.begin());

      //if we've found it, update lru and return
      if (it!=_map.end()) {
	_lru.pop_front();
	_lru.splice(it->first,_lru,_lru.begin());
	return it->second;
      }

      //otherwise, call _f and do an insertion
      it=_map.insert(make_pair(_lru.begin(),_f(x))).first;

      //if full, remove least-recently-used
      if (_map.size()>_n) {
	_map.erase(--_lru.end());
        _lru.pop_back();
      }

      assert(_map.size()<=_n);
      assert(_lru.size()==_map.size());

      //return the result
      return it->second;
    }
  protected:
    size_type _n;
    mutable map _map;
    F _f;
    mutable list _lru;
  };
  
} //~namespace util

#endif
