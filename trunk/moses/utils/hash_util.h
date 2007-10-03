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

#ifndef HASH_UTIL_H
#define HASH_UTIL_H

#include <ext/hash_map>
#include <ext/functional>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/functional/hash.hpp>
#include <boost/bind.hpp>
#include "func_util.h"
#include "tree.h"

template<typename Hash>
struct ptr_hash_val {
  ptr_hash_val(const Hash& h=Hash()) : _h(h) { }
  template<typename T>
  size_t operator()(const T* t) const { return _h(*t); }
  template<typename T1,typename T2>
  bool operator()(const T1* t1,const T2* t2) const { 
    return (*t1==*t2); 
  }
private:
  Hash _h;
};

struct ptr_hash_ref {
  template<typename T>
  size_t operator()(const T* t) const { return (size_t)t; }
};

namespace __gnu_cxx {
  template<> struct hash<std::string> {
    size_t operator()(const std::string& str) const {
      return hash<const char*>()(str.c_str());
    }
  };

  //override with nicer alternative
  template<typename T>
  struct hash<tree<T> > {
    size_t operator()(const tree<T>& tr) const { 
      return boost::hash_range(tr.begin(),tr.end());
    }
  };

  template <class Key, 
	    class T,
	    class Hash  = hash<Key>,
	    class Equal = equal_to<Key>,
	    class Alloc =  allocator<T> >
  class random_access_hash_map {
  public:
    typedef std::pair<Key,T> value_type;
    typedef typename vector<value_type>::iterator iterator;
    typedef typename vector<value_type>::const_iterator const_iterator;
    typedef hash_map<const Key*,iterator,
		     ptr_hash_val<Hash>,ptr_hash_val<Hash>,Alloc> hash_map;
    typedef typename vector<value_type>::size_type size_type;

    iterator begin() { return _values.begin(); }
    const_iterator begin() const { return _values.begin(); }
    iterator end() { return _values.end(); }
    const_iterator end() const { return _values.end(); }

    iterator find(const Key& x) {
      typename hash_map::iterator it=_map.find(&x);
      return (it==_map.end() ? _values.end() : it->second);
      
    }
    const_iterator find(const Key& x) const {
      typename hash_map::const_iterator it=_map.find(&x);
      return (it==_map.end() ? _values.end() : it->second);
    }

    std::pair<iterator,bool> insert(const value_type& x) {
      typename hash_map::const_iterator it=_map.find(&x.first);
      if (it!=_map.end())
        return make_pair(it->second,false);
      if (_values.size()>=_values.capacity()) {
	_values.push_back(x);
	reset();
      } else {
	_values.push_back(x);
	_map.insert(make_pair(&_values.back().first,--_values.end()));
      }
      return make_pair(--_values.end(),true);
    }
    void replace(iterator it,const value_type& x) {
      _map.erase(&(it->first));
      *it=x;
      _map.insert(make_pair(&(it->first),it));
    }

    size_type size() const { return _values.size(); }
    void clear() { _values.clear(); _map.clear(); }

    void reserve(size_type sz) { 
      if (sz>_values.capacity()) {
	_values.reserve(sz);
	reset();
      }
    }
    size_type capacity() const { return _values.capacity(); }

    value_type& back() { return _values.back(); }
    const value_type& back() const { return _values.back(); }
    bool empty() const { return _values.empty(); }

    value_type& operator[](size_type idx) { return _values[idx]; }
    const value_type& operator[](size_type idx) const { return _values[idx]; }

    void reset() {
      using namespace boost;
      using namespace std;
      _map.clear();
      for (typename vector<value_type>::iterator it=_values.begin();
	   it!=_values.end();++it)
      _map.insert(make_pair(&(it->first),it));
    }
  private:
    vector<value_type> _values;
    hash_map _map;
  };

} //~namespace __gnu_cxx

#endif

    /***
    typedef typename hash_map::key_type key_type;
    typedef T data_type;
    typedef T mapped_type;
    typedef typename hash_map::value_type value_type;
    typedef typename hash_map::hasher hasher;
    typedef typename hash_map::key_equal key_equal;
  

    typedef typename hash_map::difference_type difference_type;
    typedef typename hash_map::pointer pointer;
    typedef typename hash_map::const_pointer const_pointer;
    typedef typename hash_map::reference reference;
    typedef typename hash_map::const_reference const_reference;

    typedef typename hash_map::iterator iterator;
    typedef typename hash_map::const_iterator const_iterator;
    
    typedef typename hash_map::allocator_type allocator_type;
    **/
