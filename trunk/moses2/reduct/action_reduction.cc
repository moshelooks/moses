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
#include "reduct/action_rules.h"

namespace reduct {

  const rule& action_reduction() {
    static iterative r;
    
    r =
      iterative(sequential(downwards(level()),
			   downwards(reduce_action_action_if()),
			   downwards(reduce_const_cond_action_if()),
			   downwards(reduce_const_action_seq()),
			   downwards(reduce_empty_arg_seq())
			   ));

    return r;
  }
}

