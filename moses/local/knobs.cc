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

#include "knobs.h"
#include <boost/iterator/counting_iterator.hpp>

namespace local {
  int knob_count(kpre_it ks) {
    return ks->size()+accumulate
      (make_transform_iterator(make_counting_iterator(ks.begin()),knob_count),
       make_transform_iterator(make_counting_iterator(ks.end()),knob_count),0);
  }
} //~namespace local
