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

#ifndef _REDUCT_CONTIN_RULES_H
#define _REDUCT_CONTIN_RULES_H

#include "reduct/reduct.h"

namespace reduct {

  //x+0 -> x
  struct reduce_plus_zero : public crule<reduce_plus_zero> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //x*1 -> x
  //x*0 -> 0
  struct reduce_times_one_zero : public crule<reduce_times_one_zero> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //x/z+y/z -> (x+y)/z
  //or more generally, sum 1/z*x_i + sum y_j -> 1/z*(sum x_i) + sum y_j
  //when sevelar choices are possible the chosen one :
  //1)is the one that shorten the most the expression
  //2)if not unique, the lowest one according to the index order
  struct reduce_factorize_fraction : public crule<reduce_factorize_fraction> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //x*y+x*z -> x(y+z)
  //or more generally, sum x*y_i + sum z_j -> x*(sum y_i) + sum z_j
  //when sevelar choices are possible the chosen one :
  //1)is the one that shorten the most the expression
  //2)if not unique, the lowest one according to the index order
  //Note : if x is numerator under div, it works too
  struct reduce_factorize : public crule<reduce_factorize> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //x/c -> 1/c * x
  //x/(c*y) -> 1/c *x/y
  //x/0 -> 1 DELETED BECAUSE NO PROTECTION ANYMORE
  //0/x -> 0
  struct reduce_invert_constant : public crule<reduce_invert_constant> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };
  
  //(x*y)/(x*z) -> y/z
  //or more generally,
  //(prod x_i*prod y_j)/(prod x_i*prod z_k)-> prod y_j/prod z_k
  struct reduce_fraction : public crule<reduce_fraction> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };
  
  //(x/y)/z -> x/(y*z)
  //x/(y/z) -> (x*z)/y
  //x*(y/z) -> (x*y)/z,
  //more generally prod x_i * prod y_j/z_j -> (prod x_i * prod y_j)/(prod z_j)
  struct reduce_times_div : public crule<reduce_times_div> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //+(x) -> x
  //*(x) -> x
  struct reduce_plus_times_one_child : public crule<reduce_plus_times_one_child> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //log(x)+log(y) -> log(x*y), log(x)-log(y) -> log(x/y)
  //or more generally
  //sum log(x_i) - sum log(y_j) -> log((prod x_i)/(prod y_j))
  //works only if at least one log(x_i) exists otherwise
  //there would be a conflict with the rule log(c/x) -> -log((1/c)*x)
  struct reduce_sum_log : public crule<reduce_sum_log> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };
  
  //log(c/x) -> -log(c^1*x)
  //and also
  //log(exp(x)*y) -> x+log(y)
  //or more generally log(prod exp(x_i)*prod y_j) -> sum x_i +log(prod y_j)
  struct reduce_log_div_times : public crule<reduce_log_div_times> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

  //prod exp(x_i) -> exp(sum x_i)
  struct reduce_exp_times : public crule<reduce_exp_times> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };
   
  //x/exp(y) -> x*exp(-y)
  struct reduce_exp_div : public crule<reduce_exp_div> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };
  
  //exp(log(x)+y) -> x*exp(y)
  //or more generally, exp(sum log(x_i) + sum y_j) -> prod x_i * exp(sum y_j)
  struct reduce_exp_log : public crule<reduce_exp_log> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };
  
  //sin(x + c) -> sin(x + (c>pi? c-pi : (c<= pi? c+pi))
  //or more generally
  //sin(sum x_i + sum c_j) -> sin(sum x_i + ((sum c_j)+pi)%2pi -pi
  struct reduce_sin : public crule<reduce_sin> {
    void operator()(vtree& tr,vtree::iterator it) const;
  };

} //~namespace reduct

#endif
