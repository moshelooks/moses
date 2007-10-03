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

#ifndef _UTIL_HASHING_H
#define _UTIL_HASHING_H

#include <boost/functional/hash.hpp>
#include "util/tree.h"

namespace util {

  template<typename T,
	   typename Hash=boost::hash<T> >
  struct deref_hash {
    deref_hash(const Hash& h=Hash()) : hash(h) {} 
    size_t operator()(const T& t) const { return hash(*t); }
    Hash hash;
  };

  template<typename T,
	   typename Equals=std::equal_to<T> >
  struct deref_equals {
    deref_equals(const Equals& e=Equals()) : equals(e) {} 
    bool operator()(const T& x,const T& y) const { return equals(*x,*y); }
    Equals equals;
  };

  template<typename T>
  std::size_t hash_value(const tree<T>& tr) { 
    return boost::hash_range(tr.begin(),tr.end());
  }

} //~namespace util

#endif
