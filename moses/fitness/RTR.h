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

#ifndef RTR_H
#define RTR_H

#include "math_util.h"
#include "basic_types.h"
#include "selection.h"
#include <iterator>

namespace fitness {

  template<typename Distance>
  class RTR {
  public:
    int windowSize;

    RTR(int ws) : windowSize(ws) { }
    RTR(int ws,const Distance& d) : windowSize(ws),_d(d) { }

    template<typename It,typename T,typename Comp>
    It operator()(It from,It to,const T& t,fitness_t f,const Comp& c) const {
      assert(distance(from,to)>=windowSize);

      /*if (windowSize==1) {
	It x=from;
	std::advance(x,select());
	return ((*x<t) ? x : to);
	}*/

      Selector select(distance(from,to));
      It closest;
      typename Distance::result_type closestD=NEG_INFINITY;
      for (int i=0;i<windowSize;++i) {
	It x=from+select();
	typename Distance::result_type d=_d(t,x->first);
	if (d>closestD) {
	  d=closestD;
	  closest=x;
	}
      }
      return (c(closest->second,t) ? closest : to);
    }

    template<typename It,typename T>
    It operator()(It from,It to,score_t s,const T& t) const {
      assert(distance(from,to)>=windowSize);
      Selector select(distance(from,to));
      It closest;
      typename Distance::result_type closestD=NEG_INFINITY;
      for (int i=0;i<windowSize;++i) {
	It x=from+select();
	typename Distance::result_type d=_d(t,x->second);
	if (d>closestD) {
	  d=closestD;
	  closest=x;
	}
      }
      if (s>closest->first) {
	closest->first=s;
	closest->second=t;
	return closest;
      }
      return to;
    }

    template<typename It>
    void operator()(It srcfrom,It srcto,It dstfrom,It dstto) const {
      for (;srcfrom!=srcto;++srcfrom)
	operator()(dstfrom,dstto,srcfrom->first,srcfrom->second);
    }      

    const Distance& metric() const { return _d; }
  private:
    const Distance& _d;
  };

  template<typename Distance>
  RTR<Distance> make_rtr(int ws,const Distance& d) {
    return RTR<Distance>(ws,d);
  }  
} //~namespace fitness

#endif
