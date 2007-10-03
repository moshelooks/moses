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

#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <algorithm>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <climits>

#define NEG_INFINITY INT_MIN
#define POS_INFINITY INT_MAX
#define PI 3.14159265

typedef int disc_t;
typedef float contin_t;

//random float in [0,1]
inline float randfloat() {
  return (float)rand()/(float)RAND_MAX;
}
//random double in [0,1]
inline float randdouble() {
  return double(rand())/double(RAND_MAX);
}
//random int in [0,n)
inline int randint(int n) {
  return rand()%n;
}

//random boolean
inline bool randbool() {
  return rand()%2==0;
}
//random gaussian
inline float randgauss(float mean, float sd) {
  return mean+sd*sin(randfloat()*2*PI) * sqrt(-2*log(randfloat()));
}

template<typename It,typename Accessor>
float mean(It begin,It end,const Accessor& acc) {
  float res=0;
  for (It it=begin;it!=end;++it)
    res+=acc(*it);
  return res/(float)distance(begin,end);
}

template<typename It,typename Accessor>
float standardDeviation(It begin,It end,const Accessor& acc) {
  int length=distance(begin,end);
  if (length<2)
    return 0.0;
  float minVal=POS_INFINITY,maxVal=NEG_INFINITY;
  for (It it=begin;it!=end;++it) {
    float value=acc(*it);
    minVal=std::min(minVal,value);
    maxVal=std::max(maxVal,value);
  }
  return (maxVal-minVal)/((float)length-1.0);
}

class RandomGenerator {
public:
  virtual ~RandomGenerator() { }
  virtual float operator()() const=0;
};

class RandomGauss : public RandomGenerator {
public:
  RandomGauss(float mean1=0.0,float sd1=0.1) : mean(mean1),sd(sd1) { }
  const float mean;
  const float sd;
  float operator()() const { return randgauss(mean,sd); }
};

class RandomUniform : public RandomGenerator {
public:
  RandomUniform(float minX1,float maxX1) : minX(minX1),maxX(maxX1) { }
  const float minX;
  const float maxX;
  float operator()() const { return (randfloat()*(maxX-minX))+minX; }
};

class AdjacentNegativesComp {
public:
  bool operator()(int a,int b) const {
    return (a==-b) ? a<b : abs(a)<abs(b);
  }
};

namespace std {
  template <class _ForwardIter>
  bool is_sorted(_ForwardIter __first, _ForwardIter __last)
  {
    if (__first == __last)
      return true;
    
    _ForwardIter __next = __first;
    for (++__next; __next != __last; __first = __next, ++__next) {
      if (*__next < *__first)
	return false;
    }
    
    return true;
  }
}

//returns true iff first[i] -> second[i]
template<typename It1,typename It2>
bool implies(It1 from1,It1 to1,It2 from2) {
  for (;from1!=to1;++from1,++from2)
    if (*from1 && !(*from2))
      return false;
  return true;
}

unsigned long int catalan(unsigned int n);

int select_by_catalan(unsigned int n);

template<typename It>
void compute_catalans(It from,It to,unsigned long int value,int idx) {
  for (;from!=to;++idx,++from)
    *from = value = (value*(2*idx-1)*2)/(idx+1);
}
template<typename It>
void compute_catalans(It from,It to) {
  if (from==to)
    return;
  *from=1;
  compute_catalans(++from,to,1,1);
}

template<typename It>
void compute_logs_of_catalans(It from,It to,double value,int idx) {
  for (;from!=to;++idx,++from)
    *from = value = value + log2(double(2*idx-1)/double(idx+1)) + double(1);
}
template<typename It>
void compute_logs_of_catalans(It from,It to) {
  if (from==to)
    return;
  *from=double(0);
  compute_logs_of_catalans(++from,to,double(0),1);
}

#endif
