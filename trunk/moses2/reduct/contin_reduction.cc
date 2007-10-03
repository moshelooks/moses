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

#include "reduct/reduct.h"
#include "reduct/meta_rules.h"
#include "reduct/general_rules.h"
#include "reduct/contin_rules.h"

namespace reduct {
  const rule& contin_reduction() {
    static iterative r=
      iterative(sequential(downwards(level()),
			   upwards(eval_constants()),
			       
			   downwards(reduce_plus_times_one_child()),
			   downwards(reduce_plus_zero()),
			   downwards(reduce_times_one_zero()),
			   downwards(reduce_sin()),
			   downwards(reduce_invert_constant()),

			   downwards(reduce_log_div_times()),
			   downwards(reduce_exp_times()),
			   downwards(reduce_exp_div()),
			   downwards(reduce_exp_log()),
			   downwards(reduce_times_div()),
			   downwards(reduce_sum_log()),
			   
			   upwards(reorder_commutative()),
			   downwards(reduce_factorize()),
			   downwards(reduce_factorize_fraction()),
			   downwards(reduce_fraction())));
    return r;
  }
} //~namespace reduct
