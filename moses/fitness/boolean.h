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

#ifndef BOOLEAN_H
#define BOOLEAN_H

#include "bool_util.h"
#include <functional>
#include <ext/numeric>
#include "tree.h"
#include <vector>
#include <boost/functional/hash.hpp>
#include <iterator>
#include <iostream>

#include "Vertex.h"

namespace fitness {
  using namespace id;
  using namespace std;
  using namespace __gnu_cxx;
  using namespace boost;

  struct EvenParity {
  public:
    template<typename It>
    bool operator()(It from,It to) const {
      return (accumulate(from,to,0)%2==0);
    }
  };

  struct Multiplex {
  public:
    Multiplex(int a) : arity(a) { }
    int arity;
    template<typename It>
    bool operator()(It from,It to) const {
      int addr=0;
      for (int i=0;i<arity;++i)
	if (*from++)
	  addr+=(1<<i);
      return (*(from+addr));
    }
  };

  struct MultiplexedParities {
  public:
    MultiplexedParities(int a,int b) : parity(a),mp(b) { }
    int parity;
    Multiplex mp;
    template<typename It>
    bool operator()(It from,It to) const {
      /*cout << "***" << endl;
      for (It it=from;it!=to;++it)
	cout << bool(*it) << " ";
	cout << endl;*/
	
      vector<bool> tmp(mp.arity+(1<<mp.arity),false);
      for (vector<bool>::iterator it=tmp.begin();
	   it!=tmp.end();++it,from+=parity) {
	*it=EvenParity()(from,from+parity);
	//cout << bool(*it) << " ";
      }
      /**cout << endl;
	 cout << " -> " << mp(tmp.begin(),tmp.end()) << endl;**/

      return mp(tmp.begin(),tmp.end());
    }
  };

  template<typename T,typename Mapper>
  class Boolean : public unary_function<tree<T>,float> {
  public:
    Boolean(const Mapper& mapper,int arity,int nCases=0) : 
      _mapper(mapper),_arity(arity),
      _nCases(nCases==0 ? power(2,arity) : nCases) { }

    int operator()(const tree<T>& t) const {
      //cout << "got " << t << endl;
      return (t.empty() ? NEG_INFINITY : operator()(t.begin()));
    }
    template<typename iter>
    int operator()(iter src) const {
      int f=0;
      
      /**for (int i=0;i<512;++i) {
        vector<bool> v(_arity);
        for (int j=0;j<_arity;++j)
          v[j]=randbool();
        f-=(bool_evaluate(v,src)!=_mapper(v.begin(),v.end()));
	}
      **/

      for (int i=0;i<_nCases;++i) {
	vector<bool> v(_arity);
	for (int j=0;j<_arity;++j)
	  v[j]=(i>>j)%2;
	f-=(bool_evaluate(v,src)!=_mapper(v.begin(),v.end()));
      }

      return f;
    }

    pair<bool,bool> dominates(const tree<T>& x,const tree<T>& y) const {
      /**if (y.empty())
	return (x.empty() ? make_pair(false,false) : make_pair();
      else if (x.empty())
      return -1;**/
      assert(!x.empty() && !y.empty());

      bool xDoms=false,yDoms=false;
      for (int i=0;i<_nCases;++i) {
	vector<bool> v(_arity);
	for (int j=0;j<_arity;++j)
	  v[j]=(i>>j)%2;

	bool correct=_mapper(v.begin(),v.end());
	bool xCorrect=(bool_evaluate(v,x.begin())==correct);
	bool yCorrect=(bool_evaluate(v,y.begin())==correct);

	if (xCorrect && !yCorrect) {
	  if (yDoms)
	    return make_pair(true,true);
	  xDoms=true;
	} else if (!xCorrect && yCorrect) {
	  if (xDoms)
	    return make_pair(true,true);
	  yDoms=true;
	}
      }
      assert(!(xDoms && yDoms));
      return make_pair(xDoms,yDoms);
    }
  private:
    const Mapper& _mapper;
    const int _arity;
    const int _nCases;
  };

  template<typename T>
  struct CaseBasedBoolean : public unary_function<tree<T>,float> {
    CaseBasedBoolean() { }
    CaseBasedBoolean(istream& in) {
      while (in.good()) {
	char foo[10000];
	in.getline(foo,10000);
	string str(foo);
	if (!in.good() || str.empty())
	  break;
	
	vector<bool> tmp;
	stringstream ss(str);
	bool v;
	ss >> v;
	while (ss.good()) {
	  bool x;
	  ss >> x;
	  tmp.push_back(x);
	}
	tmp.push_back(v); //put the result last;
	
	if (!_cases.empty())
	  assert(_cases.front().size()==tmp.size());
	
	_cases.push_back(tmp);
      }
      cout << "#cases: " << _cases.size() << " arity " << arity() << endl;
    }  

    fitness_t operator()(const tree<T>& t) const {
      /*fitness_t penalty=min(fitness_t(10-mycomplexity(t.begin()))/2.0f,
	fitness_t(0));*/
      fitness_t penalty=fitness_t(-mycomplexity(t.begin()))/2.0f;

      return 
  	 fitness_t(t.empty() ? NEG_INFINITY : 
		   2.0f*(fitness_t(operator()(t.begin()))+penalty));
    }

    template<typename iter>
    int operator()(iter src) const {
      //cout << "cbb" << endl;

      int f=0;
      for (CaseSeq::const_iterator c=_cases.begin();c!=_cases.end();++c) {
	f-=(bool_evaluate(*c,src)!=(*c)[arity()]);
	//cout << bool_evaluate(*c,src) << " " << (*c)[arity()] << endl;
      }
      return f;
    }

    int arity() const { return _cases.front().size()-1; }

    pair<bool,bool> dominates(const tree<T>& x,const tree<T>& y) const {
      assert(!x.empty() && !y.empty());

      bool xDoms=false,yDoms=false;
      for (CaseSeq::const_iterator c=_cases.begin();c!=_cases.end();++c) {
	bool correct=(*c)[arity()];
	bool xCorrect=(bool_evaluate(*c,x.begin())==correct);
	bool yCorrect=(bool_evaluate(*c,y.begin())==correct);

	if (xCorrect && !yCorrect) {
	  if (yDoms)
	    return make_pair(true,true);
	  xDoms=true;
	} else if (!xCorrect && yCorrect) {
	  if (xDoms)
	    return make_pair(true,true);
	  yDoms=true;
	}
      }
      assert(!(xDoms && yDoms));
      return make_pair(xDoms,yDoms);
    }

  protected:
    typedef vector<vector<bool> > CaseSeq;
    CaseSeq _cases;
  };

  typedef std::vector<pair<pair<int,int>,double> > NearSet;

  template<typename iter>
  double fuzz(const NearSet& ns,int x,unsigned int idx,iter it) {
    using namespace id;
    
    int 
      arg1=get_argument_idx(*it.begin()),
      arg2=get_argument_idx(*++it.begin());
  
    if (is_identity(*it)) {
      if (arg1==arg2 || arg1==3 || arg2==3)
	return double(1);
      return (ns[idx].first.first==ns[idx].first.second) ? 
	double(1) : double(0);
    }

    assert(is_near(*it));

    //cases: near(id1,id1) near(id2,id2)
    //       near(x,x)     
    //       near(id1,x)   near(x,id1)
    //       near(id2,x)   near(x,id2)
    //       near(id1,id2) near(id2,id1)

    if (arg1==1 && arg2==2) //one
      return double(0);

    for (unsigned int i=0;i<ns.size();++i) {
      if (i==idx)
	continue;

      if ((arg1==1 && arg2==1 && ns[i].first.first==ns[i].first.second && 
	   ns[i].first.second==ns[idx].first.first) || //two
	  (arg1==2 && arg2==2 && ns[i].first.first==ns[i].first.second && 
	   ns[i].first.second==ns[idx].first.second) || //three
	  (arg1==3 && arg2==3 && ns[i].first.first==ns[i].first.second &&
	   ns[i].first.second==x) || //four
	  (arg1==2 && arg2==1 && ns[i].first.first==ns[idx].first.second &&
	   ns[i].first.second==ns[idx].first.first) || //five
	  (arg1==1 && arg2==3 && ns[i].first.first==ns[idx].first.first &&
	   ns[i].first.second==x) || //six
	  (arg1==3 && arg2==1 && ns[i].first.first==x &&
	   ns[i].first.second==ns[idx].first.first) || //seven
	  (arg1==2 && arg2==3 && ns[i].first.first==ns[idx].first.second &&
	   ns[i].first.second==x) || //eight
	  (arg1==3 && arg2==2 && ns[i].first.first==x &&
	   ns[i].first.second==ns[idx].first.second)) { //nine
	return ns[i].second;
      }
    }
    return double(0);
  }

  class Near : public unary_function<tree<Vertex>,float> {
  public:
    Near(int ni) : nItems(ni) { }
    
    const int nItems;
    NearSet cases;

    template<typename iter>
    float operator()(iter src) const {
      cout << "fitting " << tree<Vertex>(src) << endl;
      float fitness=0;
      for (unsigned int idx=0;idx<cases.size();++idx) {
	double prediction=0;
	for (int x=0;x<nItems;++x) {
	  prediction=max(prediction,
			 fuzzy_evaluate(bind(fuzz<tree<Vertex>::iterator>,
					     cases,x,idx,_1),src));
	}
	fitness-=fabs(prediction-cases[idx].second);
      }
      cout << "pingo " << fitness << endl;
      return fitness;
    }
  };
  
} //~namespace fitness

#endif
