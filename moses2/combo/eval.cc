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

#include "eval.h"

namespace combo {

  truth_table::size_type 
  truth_table::hamming_distance(const truth_table& other) const {
    assert(other.size()==size());

    size_type res=0;      
    for (const_iterator x=begin(),y=other.begin();x!=end();)
      res+=(*x++!=*y++);
    return res;
  }

  RndNumTable::RndNumTable(int sample_count, int arity) {
    //populate the matrix
    for(int i = 0; i < sample_count; ++i) {
      contin_vector cv;
      for(int j = 0; j < arity; ++j)
	cv.push_back(util::randdouble()*2.0 - 1.0); //TODO : rescale wrt
      // input interval
      _matrix.push_back(cv);
    }
  }

  contin_table::contin_table(const vtree& t, const RndNumTable& rnt) {
    for(const_cm_it i = rnt.matrix().begin(); i != rnt.matrix().end(); ++i) {
      int arg = 1;
      for(const_cv_it j = (*i).begin(); j != (*i).end(); ++j, ++arg)
	binding(arg)=*j;
      //assumption : all inputs of t are contin_t
      vertex res = eval_throws(t);
      //assumption : res is contin_t
      assert(is_contin(res));
      push_back(get_contin(res));
    }
  }

  bool contin_table::operator==(const contin_table& ct) const {
      if(ct.size() == size()) {
        const_cv_it ct_i = ct.begin();
	for(const_cv_it i = begin(); i != end(); ++i, ++ct_i) {
	  if(!util::isEqual(*i, *ct_i))
            return false;
	}
        return true;
      }
      else return false;
    }


  contin_t contin_table::abs_distance(const contin_table& other) const {
    assert(other.size()==size());

    contin_t res=0;      
    for (const_iterator x=begin(),y=other.begin();x!=end();)
      res+=fabs(*x++-*y++);
    return res;
  }

} //~namespace combo
