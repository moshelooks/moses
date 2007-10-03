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

#include "moses/types.h"

namespace moses {

  const tree_score worst_possible_score=
  std::make_pair(-(std::numeric_limits<score_t>::max()-score_t(1)),
		 -(std::numeric_limits<complexity_t>::max()-complexity_t(1)));

} //~namespace moses
