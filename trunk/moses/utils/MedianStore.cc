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

#include "MedianStore.h"

void MedianStore::addItem(int idx) {
  int l=0,r=_store.size(),x;
  do {
    x=(l+r)/2;
    if (idx<=x) {
      ++_store[x];
      r=x;
    } else
      l=x;
  } while (x!=idx);
}

void MedianStore::removeItem(int idx) {
  int l=0,r=_store.size(),x;
  do {
    x=(l+r)/2;
    if (idx<=x) {
      --_store[x];
      r=x;
    } else
      l=x;
  } while (x!=idx);
}

int MedianStore::getCountBelowEq(int idx) const {
  int l=0,r=_store.size();
  if (idx>r)
    return 0;
  if (idx<l)
    return 0;
  
  int n=0,x;
  do {
    x=(l+r)/2;
    if (idx<x)
      r=x;
    else {
      n+=_store[x];
      l=x;
    }
  } while (x!=idx);
  return n;
}

int MedianStore::getItemByCount(int count) const {
  int l=0,r=_store.size(),x;
  do {
    x=(l+r)/2;
    if (_store[x]>count) {
      r=x;
      if (l==r)
	break;
    } else {
      count-=_store[x];
      l=x;
    }
  } while (count>0);
  return x;
}


