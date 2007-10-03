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

#ifndef ID_H
#define ID_H

#include "basic_types.h"
#include <string>

namespace id {
  template<typename T>
  bool is_associative(const T&);
  template<typename T>
  bool is_commutative(const T& t);

  template<typename T>
  bool is_constant(const T&);
  template<typename T>
  bool is_variable(const T&);
  template<typename T>
  bool is_argument(const T&);

  template<typename T>
  bool is_and(const T& t);
  template<typename T>
  bool is_or(const T&);
  template<typename T>
  bool is_not(const T&);
  template<typename T>
  bool is_true(const T&);
  template<typename T>
  bool is_false(const T&);
  template<typename T>
  bool is_bool_if(const T&);

  template<typename T>
  bool is_logical_op(const T& t) { 
    return (is_and(t) || is_or(t) || is_not(t) || is_true(t) || is_false(t) ||
	    is_bool_if(t)); 
  }

  template<typename T>
  bool is_minus(const T& t) { return false; }
  template<typename T>
  bool is_cos(const T& t) { return false; }

  template<typename T>
  bool is_plus(const T& t);
  template<typename T>
  bool is_times(const T&);
  template<typename T>
  bool is_inv(const T&);
  template<typename T>
  bool is_sin(const T&);
  template<typename T>
  bool is_exp(const T&);
  template<typename T>
  bool is_log(const T&);

  template<typename T>
  bool is_ifelse(const T& t);
  template<typename T>
  bool is_holding(const T&);
  template<typename T>
  bool is_pickup(const T&);
  template<typename T>
  bool is_teacher(const T&);
  template<typename T>
  bool is_ball(const T&);
  template<typename T>
  bool is_walktowards(const T&);
  template<typename T>
  bool is_done(const T&);
  template<typename T>
  bool is_near(const T&);

  template<typename T>
  bool is_identity(const T&);

  template<typename T>
  T create_progn();
  template<typename T>
  T create_if_food();
  template<typename T>
  T create_move();
  template<typename T>
  T create_right();
  template<typename T>
  T create_left();
  template<typename T>
  T create_reversal();

  template<typename T>
  T create_plus();
  template<typename T>
  T create_times();
  template<typename T>
  T create_inv();
  template<typename T>
  T create_sin();
  template<typename T>
  T create_exp();
  template<typename T>
  T create_log();

  template<typename T>
  T create_const(contin_t);
  template<typename T>
  contin_t get_const(const T&);

  template<typename T>
  T create_near();
  template<typename T>
  T create_identity();

  template<typename T>
  T create_teacher();
  template<typename T>
  T create_ball();

  template<typename T>
  T create_holding();
  template<typename T>
  T create_walktowards();
  template<typename T>
  T create_done();
  template<typename T> 
  T create_pickup();

  template<typename T> 
  T create_ifelse();

  template<typename T>
  T create_and();
  template<typename T>
  T create_or();
  template<typename T>
  T create_not();

  template<typename T>
  T create_true();
  template<typename T>
  T create_false();
  template<typename T>
  T create_bool_if();

  template<typename T>
  T create_argument(int);

  template<typename T>
  int get_argument_idx(const T&);

} //~namespace id

#endif
