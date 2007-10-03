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

namespace reduct {
  const rule& logical_reduction() {
    using namespace combo;
    static sequential r=
      sequential(downwards(reduce_nots(),id::boolean),
		 upwards(remove_dangling_junctors()),
		 iterative(sequential(upwards(eval_logical_identities()),
				      downwards(level()),
				      downwards(insert_ands(),id::boolean),
				      subtree_to_enf(),
				      downwards(reduce_ands(),id::boolean),
				      downwards(reduce_ors(),id::boolean))),
		 downwards(remove_unary_junctors(),id::boolean));
    return r;
  }
} //~namespace reduct
