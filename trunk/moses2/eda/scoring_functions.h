/****
   Copyright 2005-2007, Moshe Looks and Novamente LLC

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

#ifndef _EDA_SCORING_FUNCTIONS_H
#define _EDA_SCORING_FUNCTIONS_H

#include "eda/field_set.h"
#include "util/numeric.h"
#include <cmath>
#include <boost/lexical_cast.hpp>

namespace eda {

  struct one_max : public unary_function<instance,int> {
    int operator()(const instance& inst) const {
      //operates directly on packed_t
      return accumulate
	(make_transform_iterator(inst.begin(),util::count_bits<packed_t>),
	 make_transform_iterator(inst.end(),util::count_bits<packed_t>),0);
    }
  };

  struct n_max : public unary_function<instance,int> {
    n_max(const field_set& fs) : fields(fs) {}
    int operator()(const instance& inst) const {    
      return accumulate(fields.begin_disc(inst),fields.end_disc(inst),0);
    }
    const field_set& fields;
  };

  struct contin_max : public unary_function<instance,contin_t> {
    contin_max(const field_set& fs) : fields(fs) {}
    contin_t operator()(const instance& inst) const {
      return accumulate(fields.begin_contin(inst),fields.end_contin(inst),
			contin_t(0));
    }
    const field_set& fields;
  };

  struct contin_uniform : public unary_function<instance,contin_t> {
    contin_uniform(const field_set& fs,contin_t minval,contin_t maxval)
      : fields(fs),target(fs.n_contin()) {
      generate(target.begin(),target.end(),
	       bind(std::plus<contin_t>(),
		  bind(std::multiplies<contin_t>(),bind(util::randdouble),
		       maxval-minval),minval));
    }
    contin_t operator()(const instance& inst) const {
      contin_t res=0;
      field_set::const_contin_iterator it1=fields.begin_contin(inst);
      for (vector<contin_t>::const_iterator it2=target.begin();
	   it2!=target.end();++it1,++it2)
	res-=fabs((*it1)-(*it2));
      return res;
    }
    const field_set& fields;
    vector<contin_t> target;
  };

  struct sphere : public unary_function<instance,contin_t> {
    sphere(const field_set& fs) : fields(fs) {}
    contin_t operator()(const instance& inst) const {
      contin_t res=0;
      for (field_set::const_contin_iterator it=fields.begin_contin(inst);
	   it!=fields.end_contin(inst);++it) {
	contin_t v=*it;
	res-=(v*v);
      }
      return res;
    }
    const field_set& fields;
  };

  struct ontomax: public unary_function<instance,contin_t> {
    ontomax(const field_set& fs) : fields(fs) {}
    contin_t operator()(const instance& inst) const {
      contin_t res=0;
      for (field_set::const_onto_iterator it=fields.begin_onto(inst);
	   it!=fields.end_onto(inst);++it) {
	onto_t s=*it;
	assert(s.length()==2);
	int a=boost::lexical_cast<int>(s[0]);
	int b=boost::lexical_cast<int>(s[1]);
	res+=a+b;
      }
      return res;
    }
    const field_set& fields;
  };

} //~namespace eda

#endif
