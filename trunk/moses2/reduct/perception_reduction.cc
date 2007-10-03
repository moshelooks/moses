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
#include "reduct/mixed_rules.h"
#include "reduct/perception_rules.h"

namespace reduct {

  const rule& perception_reduction() {
    static assum_iterative r;
    
    r = assum_iterative(sequential(downwards(level()),
				   //simple perception rules
				   downwards(reduce_irreflexive()),
				   downwards(reduce_reflexive()),
				   downwards(reduce_identity_of_indiscernibles()),

				   //generate assumptions
				   downwards(reduce_and_assumptions(r)),
				   //the following is commented because due to
				   //the fact that there is no boolean rules here
				   //the double negation cannot be reduced
				   //and it leads to an infinit recursion
				   //downwards(reduce_or_assumptions(r))
				   downwards(reduce_ultrametric()),
				   downwards(reduce_transitive()),
				   //reduce from assumptyions
				   downwards(reduce_from_assumptions(r)),
				   downwards(reduce_inequality_from_assumptions())
				   )
			);
    
    return r;
  }
}

