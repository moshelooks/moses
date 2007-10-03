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

#ifndef _EDA_SCORING_H
#define _EDA_SCORING_H

#include "eda/eda.h"
#include "util/functional.h"

namespace eda {
  
  template<typename ScoreT>
  struct scored_instance : public util::tagged_item<instance,ScoreT> {
    typedef util::tagged_item<instance,ScoreT> super;

    scored_instance(const instance& i,const ScoreT& s) : super(i,s) { }
    scored_instance(const instance& i) : super(i) { }
    scored_instance() { }
    template<class T1, class T2>
    scored_instance(const std::pair<T1,T2>& p) : super(p) { }
  };

  template<typename Scoring>
  scored_instance<typename result_of<Scoring(instance)>::type> 
  score_instance(const instance& inst,const Scoring& score) {
    return 
      scored_instance<typename result_of<Scoring(instance)>::type>(inst,
								   score(inst));
  }

} //~namespace eda

#endif
