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

#ifndef FORMULAE_H
#define FORMULAE_H

#include <iostream>
#include <string>
#include <numeric>
#include <vector>
#include <ext/numeric>
#include <boost/lexical_cast.hpp>
#include "id.h"
#include "io_util.h"

namespace fitness {
  using namespace trees;
  using namespace std;
  using namespace __gnu_cxx;
  using namespace boost;

  template<typename T>
  class EvenParity : public MetaData<T>,std::unary_function<tree<T>,float> {
  public:
    EvenParity(int k,int nCases=0) : 
      _k(k),_nCases(nCases==0 ? power(2,k) : nCases) { }
			
    template<typename It>
    bool operator()(It from,It to) const {
      return (accumulate(from,to,0)%2==0);
    }
    
    float operator()(const tree<T>& t) const {
      //cout << "top, " << t << endl;
      if (t.empty())
	return NEG_INFINITY;
      float f=0;
      for (int i=0;i<_nCases;++i) {
	vector<bool> v(_k);
	for (int j=0;j<_k;++j) {
	  v[j]=(i>>j)%2;
	  bindBool("x"+lexical_cast<string>(j),v[j]);
	  bindBool("!x"+lexical_cast<string>(j),!v[j]);
	}
	T result=evaluate(t);
	assert(is_true(result) || is_false(result));
	f-=(is_true(result)!=operator()(v.begin(),v.end()));
      }
      return f;
    }

    void printTruthTable(const tree<T>& t) {
      for (int i=0;i<16;++i) {
	bool p=(i)%2,q=(i>>1)%2,r=(i>>2)%2,s=(i>>3)%2;

	set_binding("p",p ? create_true() : create_false());
	set_binding("q",q ? create_true() : create_false());
	set_binding("r",r ? create_true() : create_false());
	set_binding("s",s ? create_true() : create_false());

	set_binding("x0",p ? create_true() : create_false());
	set_binding("x1",q ? create_true() : create_false());
	set_binding("x2",r ? create_true() : create_false());
	set_binding("x3",s ? create_true() : create_false());

	//cout << evaluate(t) << " ";
      }
    }
  private:
    void bindBool(string s,bool v) const {
      v ? set_binding(s,create_true()) : set_binding(s,create_false());
    }
    const int _k;
    const int _nCases;
  };

  template<typename T>
  class SymReg : public MetaData<T>,std::unary_function<tree<T>,float> {
  public:
    typedef vector<pair<float,float> > CaseSeq;
    
    SymReg(int nCases=20) : _cases(nCases) {
      for (CaseSeq::iterator it=_cases.begin();it!=_cases.end();++it) {
	it->first=randfloat()*2-1;
	it->second=it->first+it->first*(it->first+
					it->first*
					(it->first+it->first*it->first));
      }
    }
			
    float operator()(const tree<T>& t) const {
      //cout << "top, " << t << endl;
      if (t.empty())
	return NEG_INFINITY;
      float f=0;
      for (CaseSeq::const_iterator it=_cases.begin();it!=_cases.end();++it) {
	set_binding("x",create_const(it->first));
	T result=evaluate(t);
	assert(this->is_constant(result));
	f-=fabs(it->second-get_const(result));
      }
      return f;
    }
  private:
    CaseSeq _cases;
  };

} //~namespace fitness

#endif
