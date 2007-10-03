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
#include "reduct/logical_rules.h"
#include "reduct/contin_rules.h"
#include "reduct/mixed_rules.h"

namespace reduct {
  const rule& mixed_reduction() {
    static iterative r_without_reduce_gt_zero_prod;

    //This set of rule is defined to avoid infinit recursion of the rule
    //reduce_gt_zero_prod
    r_without_reduce_gt_zero_prod =
        iterative(sequential(//general
			   downwards(level()),
			   upwards(eval_constants()),

			   //simple mixed
			   downwards(reduce_gt_zero_log()),
			   downwards(reduce_gt_zero_exp()),
			   downwards(reduce_gt_zero_minus_exp()),
			   //downwards(reduce_gt_zero_times_const()),//deprecat
			   //downwards(reduce_gt_zero_const_div()),//deprecat
			   //downwards(reduce_gt_zero_prod_exp()),//deprecat
			   //downwards(reduce_gt_zero_const_sum_sin()), //deprecat
			   downwards(reduce_gt_zero_impulse()),
			   downwards(reduce_impulse_sum()),
			   downwards(reduce_contin_if_to_impulse()),

			   //complex mixed
 			   upwards(reorder_commutative()),
 			   downwards(reduce_gt_zero_pair_power()),
 			   downwards(reduce_impulse_power()),
 			   downwards(reduce_impulse_prod()),
 			   downwards(reduce_contin_if()),
 			   downwards(reduce_op_contin_if()),
 			   downwards(reduce_contin_if_equal_branch()),
 			   downwards(reduce_contin_if_inner_op()),
 			   downwards(reduce_contin_if_substitute_cond()),
 			   downwards(reduce_junction_gt_zero_sum_constant()),

			   //very complex mixed
 			   downwards(reduce_from_assumptions
				     (r_without_reduce_gt_zero_prod)),
			   downwards(reduce_inequality_from_assumptions()),
 			   downwards(reduce_contin_if_not
				     (r_without_reduce_gt_zero_prod)),
 			   downwards(reduce_gt_zero_sum
				     (r_without_reduce_gt_zero_prod)),
			   //commented to avoid infinit recursivity
 			   //downwards(reduce_gt_zero_prod(r)),
 			   downwards(reduce_gt_zero_div
				     (r_without_reduce_gt_zero_prod)),
 			   downwards(reduce_gt_zero_sum_sin
				     (r_without_reduce_gt_zero_prod)),
 			   downwards(reduce_gt_zero_sin
				     (r_without_reduce_gt_zero_prod)),
 			   downwards(reduce_and_assumptions
				     (r_without_reduce_gt_zero_prod))//,
			   //the following is commented because due to
			   //the fact that there is no boolean rules here
			   //the double negation cannot be reduced
			   //and it leads to an infinit recursion
 			   //downwards(reduce_or_assumptions(r))
			   ));
    
    static iterative r;
    r =
      iterative(sequential(//general
			   downwards(level()),
			   upwards(eval_constants()),

			   //simple mixed
			   downwards(reduce_gt_zero_log()),
			   downwards(reduce_gt_zero_exp()),
			   downwards(reduce_gt_zero_minus_exp()),
			   //downwards(reduce_gt_zero_times_const()),//deprecat
			   //downwards(reduce_gt_zero_const_div()),//deprecat
			   //downwards(reduce_gt_zero_prod_exp()),//deprecat
			   //downwards(reduce_gt_zero_const_sum_sin()), //deprecat
			   downwards(reduce_gt_zero_impulse()),
			   downwards(reduce_impulse_sum()),
			   downwards(reduce_contin_if_to_impulse()),

			   //complex mixed
 			   upwards(reorder_commutative()),
 			   downwards(reduce_gt_zero_pair_power()),
 			   downwards(reduce_impulse_power()),
 			   downwards(reduce_impulse_prod()),
 			   downwards(reduce_contin_if()),
 			   downwards(reduce_op_contin_if()),
 			   downwards(reduce_contin_if_equal_branch()),
 			   downwards(reduce_contin_if_inner_op()),
 			   downwards(reduce_contin_if_substitute_cond()),
 			   downwards(reduce_junction_gt_zero_sum_constant()),

			   //very complex mixed
 			   downwards(reduce_from_assumptions(r)),
			   downwards(reduce_inequality_from_assumptions()),
 			   downwards(reduce_contin_if_not(r)),
 			   downwards(reduce_gt_zero_sum(r)),
 			   downwards(reduce_gt_zero_prod
				     (r, r_without_reduce_gt_zero_prod)),
 			   downwards(reduce_gt_zero_div(r)),
 			   downwards(reduce_gt_zero_sum_sin(r)),
 			   downwards(reduce_gt_zero_sin(r)),
 			   downwards(reduce_and_assumptions(r))//,
			   //the following is commented because due to
			   //the fact that there is no boolean rules here
			   //the double negation cannot be reduced
			   //and it leads to an infinit recursion
 			   //downwards(reduce_or_assumptions(r))
			   ));
    return r;
  }
} //~namespace reduct

