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

#ifndef BOOL_UTIL_H
#define BOOL_UTIL_H

#include <vector>
#include "id.h"
#include "tree.h"
#include <iostream>
#include <ext/numeric>
#include <boost/functional/hash.hpp>

template<typename iter>
bool evaluate_binding(const std::vector<bool>& bindings,iter it) {
  return bindings[id::get_argument_idx(*it)-1];
}

template<typename State,typename iter>
bool bool_evaluate(const State& bindings,iter it) { 
  using namespace id;
  typedef typename std::iterator_traits<iter>::value_type T;

  if (!is_logical_op(*it))
    return evaluate_binding(bindings,it);
  
  if (is_bool_if(*it))
    return (bool_evaluate(bindings,it.begin()) ? 
	    bool_evaluate(bindings,++it.begin()) :
	    bool_evaluate(bindings,--it.end()));

  if (it.begin()==it.end())
    return (is_or(*it) || is_true(*it));
  
  if (is_or(*it)) {
    for (typename tree<T>::sibling_iterator sib=it.begin();
	 sib!=it.end();++sib)
      if (bool_evaluate(bindings,sib))
	return true;
    return false;
  }
  if (is_and(*it)) {
    for (typename tree<T>::sibling_iterator sib=it.begin();
	 sib!=it.end();++sib)
      if (!bool_evaluate(bindings,sib))
	return false;
    return true;
  }
  if (is_not(*it))
    return !bool_evaluate(bindings,it.begin());
  if (is_true(*it))
    return true;
  if (is_false(*it))
    return false;

  std::cout << "can't find " << *it << std::endl;
  assert(false);
  return false;
}
template<typename T>
bool bool_evaluate(const std::vector<bool>& bindings,const tree<T>& tr) {
  return bool_evaluate(bindings,tr.begin());
}

class TruthTable {
public:
  TruthTable() { }
  template<typename It>
  TruthTable(It from,It to) : _table(from,to) { }
  template<typename T>
  TruthTable(const tree<T>& t,int arity) : _table(__gnu_cxx::power(2,arity)) {
    std::vector<bool>::iterator it=_table.begin();
    for (int i=0;it!=_table.end();++i,++it) {
      std::vector<bool> v(arity);
      for (int j=0;j<arity;++j)
	v[j]=(i>>j)%2;
      (*it)=bool_evaluate(v,t);
    }
  }
  template<typename Func>
  TruthTable(const Func& f,int arity) : _table(__gnu_cxx::power(2,arity)) {
    std::vector<bool>::iterator it=_table.begin();
    for (int i=0;it!=_table.end();++i,++it) {
      std::vector<bool> v(arity);
      for (int j=0;j<arity;++j)
	v[j]=(i>>j)%2;
      (*it)=f(v.begin(),v.end());
    }
  }
  
  template<typename It>
  bool operator()(It from,It to) {
    std::vector<bool>::const_iterator it=_table.begin();
    for (int i=1;from!=to;++from,i=i<<1)
      if (*from)
	it+=i;
    return *it;
  }

  const std::vector<bool>& table() const { return _table; }

  bool operator==(const TruthTable& tt) const { return _table==tt._table; }
private:
  std::vector<bool> _table;
};

inline std::ostream& operator<<(std::ostream& out,const TruthTable& tt) {
  for (std::vector<bool>::const_iterator it=tt.table().begin();
       it!=tt.table().end();++it)
    out << *it << " ";
  return out;
}

template<typename Binder,typename iter>
double fuzzy_evaluate(const Binder& bindings,iter it) { 
  using namespace id;
  typedef typename std::iterator_traits<iter>::value_type T;

  if (!is_logical_op(*it))
    return bindings(it);
  
  if (is_or(*it)) {
    double res=0;
    for (typename tree<T>::sibling_iterator sib=it.begin();
	 sib!=it.end();++sib)
      res=max(res,fuzzy_evaluate(bindings,sib));
    return res;
  }
  if (is_and(*it)) {
    double res=1;
    for (typename tree<T>::sibling_iterator sib=it.begin();
	 sib!=it.end();++sib)
      res=min(res,fuzzy_evaluate(bindings,sib));
    return res;
  }
  if (is_not(*it))
    return (double(1)-fuzzy_evaluate(bindings,it.begin()));
  if (is_true(*it))
    return double(1);
  if (is_false(*it))
    return double(0);
  
  std::cerr << "can't find " << *it << std::endl;
  assert(false);
  return false;
}
template<typename State,typename T>
double fuzzy_evaluate(const State& bindings,const tree<T>& tr) {
  return fuzzy_evaluate(bindings,tr.begin());
}

template<typename It1,typename It2>
int hamming_distance(It1 from1,It1 to1,It2 from2) {
  int res=0;
  while (from1!=to1)
    res+=(!(*from1++==*from2++));
  return res;
}

namespace boost {
  inline size_t hash_value(const TruthTable& tt) {
    return hash_range(tt.table().begin(),tt.table().end());
  }
}

#endif
