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

#ifndef FUNCUTIL_H
#define FUNCUTIL_H

#include <utility>
#include <queue>
#include <ext/hash_set>
#include <ext/hash_map>

template<typename T1,typename T2,typename T3>
class binary_compose2 {
public:
  typedef typename T1::result_type result_type;
  typedef typename T2::argument_type first_argument_type;
  typedef typename T3::argument_type second_argument_type;


  binary_compose2(T1 f,T2 g1,T3 g2) : _f(f),_g1(g1),_g2(g2) { }

  result_type operator()(const first_argument_type& a,
			 const second_argument_type& b) {
    return _f(_g1(a),_g2(b));
  }
private:
  T1 _f;
  T2 _g1;
  T3 _g2;
};

template<typename T1,typename T2,typename T3>
binary_compose2<T1,T2,T3> compose3(T1 f,T2 g1,T3 g2);

template<typename T1,typename T2,typename T3>
binary_compose2<T1,T2,T3> compose3(T1 f,T2 g1,T3 g2) {
  return binary_compose2<T1,T2,T3>(f,g1,g2);
}

template<typename T1,typename T2>
std::pair<T2,T1> switch_pair(const std::pair<T1,T2>& p) { 
  return std::make_pair(p.second,p.first);
}

template<typename T1,typename T2>
bool inverse_order(const std::pair<T1,T2>& a,const std::pair<T1,T2>& b) {
  return a.second<b.second ||
    (!(b.second<a.second) && a.first<b.first);
}

template<typename T,typename S,typename C>
class PQInserter {
public:
  PQInserter(std::priority_queue<T,S,C>& q) : _q(q) { }

  PQInserter& operator=(const T& t) { _q.push(t); return *this; }

  PQInserter& operator*() { return *this; }
  PQInserter& operator++() { return *this; }
  PQInserter& operator++(int) { return *this; }
private:
  std::priority_queue<T,S,C>& _q;
};
template<typename T,typename S,typename C>
PQInserter<T,S,C> inserter(std::priority_queue<T,S,C>& q) { 
  return PQInserter<T,S,C>(q); 
}

template<typename T,typename Hash,typename Eq,typename Alloc>
class HashSetInserter {
public:
  HashSetInserter(__gnu_cxx::hash_set<T>& hs) : _hs(hs) { }

  HashSetInserter& operator=(const T& t) { _hs.insert(t); return *this; }

  HashSetInserter& operator*() { return *this; }
  HashSetInserter& operator++() { return *this; }
  HashSetInserter& operator++(int) { return *this; }
private:
  __gnu_cxx::hash_set<T,Hash,Eq,Alloc>& _hs;
};
template<typename T,typename Hash,typename Eq,typename Alloc>
HashSetInserter<T,Hash,Eq,Alloc> inserter
(__gnu_cxx::hash_set<T,Hash,Eq,Alloc>& hs) { 
  return HashSetInserter<T,Hash,Eq,Alloc>(hs); 
}

template<typename Key,typename T,typename Hash,typename Eq,typename Alloc>
class HashMapInserter {
public:
  typedef typename __gnu_cxx::hash_map<Key,T,
				       Hash,Eq,Alloc>::value_type value_type;

  HashMapInserter(__gnu_cxx::hash_map<Key,T,Hash,Eq,Alloc>& hm) : _hm(hm) { }

  HashMapInserter& operator=(const value_type& v) { 
    _hm.insert(v); 
    return *this; 
  }
  HashMapInserter& operator*() { return *this; }
  HashMapInserter& operator++() { return *this; }
  HashMapInserter& operator++(int) { return *this; }
private:
  __gnu_cxx::hash_map<Key,T,Hash,Eq,Alloc>& _hm;
};
template<typename Key,typename T,typename Hash,typename Eq,typename Alloc>
HashMapInserter<Key,T,Hash,Eq,Alloc> inserter
(__gnu_cxx::hash_map<Key,T,Hash,Eq,Alloc>& hm) { 
  return HashMapInserter<Key,T,Hash,Eq,Alloc>(hm); 
}

namespace std {
  template<typename It1,typename It2,typename F>
  F for_each(It1 from1,It1 to1,It2 from2,F f) {
    for (;from1!=to1;++from1,++from2)
      f(*from1, *from2);
    return f;
  }
  template<typename It1,typename It2,typename It3,typename F>
  F for_each(It1 from1,It1 to1,It2 from2,It3 from3,F f) {
    for (;from1!=to1;++from1,++from2,++from3)
      f(*from1,*from2,*from3);
    return f;
  }
}

template<typename T>
T* ptr_to(T& t) { return &t; }

template<typename T>
struct constructor {
  typedef T result_type;

  T operator()() const { return T(); };
  template<typename A1>
  T operator()(A1& a1) const { return T(a1); }
  template<typename A1>
  T operator()(const A1& a1) const { return T(a1); }
  template<typename A1,typename A2>
  T operator()(A1& a1,A2& a2) const { return T(a1,a2); }
};

template<typename T>
struct selectn { int operator()(const T& t) const { return t.n; } };

template <class It,class Out,class Pred,class Op>
Out transform_if(It first, It last,Out result,Pred pred, Op op) {
  for ( ; first != last; ++first)
    if (pred(*first))
      *result++=op(*first);
  return result;
}

namespace func {
  struct cadr {
    template<typename T,typename U,typename V>
    const U& operator()(const std::pair<T,std::pair<U,V> >& p) const {
      return p.second.first;
    }
  };
}

#endif


