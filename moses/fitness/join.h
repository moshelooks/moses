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

#ifndef JOIN_H
#define JOIN_H

#include "id.h"

namespace fitness {
  using namespace id;

  struct Deceptive {
    Deceptive(int n,float d) : _n(n),_d(d) { }
    template<typename It>
    float operator()(It from,It to) const {
      assert(distance(from,to)%_n==0);
      float fitness=0;
      while (from!=to) {
	int u=accumulate(from,from+_n,0);
	from+=_n;
	fitness+=(u==_n ? 1.0f : (1.0f-_d)*(1.0f-float(u)/float(_n-1)));
      }
      return (fitness*12);
    }
  private:
    int _n;
    float _d;
  };

  struct Order {
    template<typename It>
    float operator()(It from,It to) const { return accumulate(from,to,0); }
  };

  template<typename T,typename Mapper>
  struct Join : public unary_function<tree<T>,float> {
    Join(const Mapper& m,int arity) : _m(m),_arity(arity) { }

    float operator()(const tree<T>& t) const {
      vector<bool> output(_arity,false),expr(_arity,false);
      for (typename tree<T>::pre_order_iterator it=t.begin();
	   it!=t.end();++it) {
	if (it.begin()==it.end() && !is_progn(*it)) { //leaf
	  int idx=get_argument_idx(*it);
	  if (idx!=0) {
	    int pos=(idx>0 ? idx : -idx);
	    if (!expr[pos-1]) { 
	      expr[pos-1]=true;
	      if (idx>0)
		output[idx-1]=true;
	    }
	  }
	}
      }
      return _m(output.begin(),output.end());
    }

    pair<bool,bool> dominates(const tree<T>& x,const tree<T>& y) const {
      float xFit=(*this)(x),yFit=(*this)(y);
      return 
	xFit>yFit ? make_pair(true,false) :
	yFit>xFit ? make_pair(false,true) : 
	make_pair(false,false);
    }

    private:
      Mapper _m;
      int _arity;
  };

  struct join_clean {
    template<typename T>
    void operator()(tree<T>& t) const {
      bool changed=false;
      for (typename tree<T>::pre_order_iterator it=++t.begin();
	   it!=t.end();) {
	if ((is_argument(*it) && get_argument_idx(*it)==0) ||
	    (is_progn(*it) && it.begin()==it.end())) {
	  it=t.erase(it);
	  changed=true;
	} else if (is_progn(*it) && ++it.begin()==it.end()) {
	  *it=*it.begin();
	  t.erase(t.flatten(it.begin()));
	} else {
	  ++it;
	}
      }
      if (changed)
	(*this)(t);
      if (t.begin().number_of_children()==1) {
	*t.begin()=*t.begin().begin();
	t.erase(t.flatten(t.begin().begin()));
      }
    }
  };
  
} //~namespace fitness

#endif
