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

#ifndef SORTUTIL_H
#define SORTUTIL_H

#include "basic_types.h"

#include <algorithm>
#include <iterator>


template<typename Op>
class RefUnary {
public:
  typedef typename Op::result_type result_type;
  typedef typename Op::argument_type argument_type;
  RefUnary(const Op& op) : _op(op) { }
  result_type operator()(const argument_type& arg) const {
    return _op(arg);
  }
private:
  const Op& _op;
};
template<typename Op>
RefUnary<Op> ref_unary(const Op& op) { return RefUnary<Op>(op); }

template<typename Op>
class RefBinary {
public:
  typedef typename Op::result_type result_type;
  typedef typename Op::argument_type argument_type;
  RefBinary(const Op& op) : _op(op) { }
  result_type operator()(const argument_type& arg1,const argument_type& arg2) const {
    return _op(arg1,arg2);
  }
private:
  const Op& _op;
};
template<typename Op>
RefBinary<Op> ref_binary(const Op& op) { return RefBinary<Op>(op); }

template<typename It,typename Bucket>
void bucket_sort(It begin,It end,const Bucket& bucket,int nBucket);
template<typename It,typename Bucket,typename SecondarySort>
void bucket_sort(It begin,It end,const Bucket& bucket,int nBucket,const SecondarySort& secondary);

template<typename It,typename Predicate>
It inplace_partition(It left,It right,const Predicate& goesRight);

template<typename It,typename Bucket>
void bucket_sort(It begin,It end,const Bucket& bucket,int nBucket) {
  using std::vector;
  typedef typename std::iterator_traits<It>::value_type Value;
  vector<Value>* lists=new vector<Value>[nBucket];
  int nReserve=2*distance(begin,end)/nBucket;
  for (int i=0;i<nBucket;++i)
    lists[i].reserve(nReserve);
  for (typename vector<Value>::const_iterator it=begin;it!=end;++it)
    lists[bucket(*it)].push_back(*it);
  typename vector<Value>::iterator at=begin;
  for (int i=0;i<nBucket;++i)
    at=copy(lists[i].begin(),lists[i].end(),at);

  delete[] lists;
}

template<typename It,typename Bucket,typename SecondarySort>
void bucket_sort(It begin,It end,const Bucket& bucket,int nBucket,const SecondarySort& secondary) {
  using std::vector
  typedef typename std::iterator_traits<It>::value_type Value;
  vector<Value>* lists=new vector<Value>[nBucket];
  int nReserve=2*distance(begin,end)/nBucket;
  for (int i=0;i<nBucket;++i)
    lists[i].reserve(nReserve);
  for (typename vector<Value>::const_iterator it=begin;it!=end;++it)
    lists[bucket(*it)].push_back(*it);
  typename vector<Value>::iterator at=begin;
  for (int i=0;i<nBucket;++i) {
    secondary(lists[i].begin(),lists[i].end());
    at=copy(lists[i].begin(),lists[i].end(),at);
  }
  delete[] lists;
}

template<typename It,typename Predicate>
It inplace_partition(It left,It right,const Predicate& goesRight) {
  if (left==right)
    return left;
  --right;
  while (left<right) {
    while (left<right && !goesRight(*left))
      ++left;
    while (left<right && goesRight(*right))
      --right;
    if (left<right) {
      std::swap(*left,*right);
      ++left;
      --right;
    }
  }
  if (!goesRight(*left))
    ++left;

  return left;
}

template<typename Comp>
struct SortAdapter {
  SortAdapter(const Comp& comp1) : comp(comp1) { }
  const Comp& comp;
  template<typename It>
  void operator()(It begin,It end) const {
    sort(begin,end,comp);
  }
};
template<typename Comp>
SortAdapter<Comp> sort_adapter(const Comp& comp) {
  return SortAdapter<Comp>(comp);
}

template<typename Comp>
struct BucketSortAdapter {
  BucketSortAdapter(const Comp& comp1,int nBucket1) : comp(comp1),nBucket(nBucket1) { }
  const Comp& comp;
  const int nBucket;
  template<typename It>
  void operator()(It begin,It end) const {
    bucket_sort(begin,end,comp,nBucket);
  }
};
template<typename Comp>
BucketSortAdapter<Comp> bucket_sort_adapter(const Comp& comp,int nBucket) {
  return BucketSortAdapter<Comp>(comp,nBucket);
}
#endif
