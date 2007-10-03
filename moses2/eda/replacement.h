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

#ifndef _EDA_REPLACE_H
#define _EDA_REPLACE_H

#include "eda/using.h"
#include "eda/field_set.h"
#include "eda/scoring.h"
#include "util/lazy_random_selector.h"

namespace eda {

  //note that NewInst only models InputIterator

  struct replace_the_worst {
    template<typename NewInst,typename Dst>
    void operator()(NewInst from,NewInst to,Dst from_dst,Dst to_dst) const {
      assert(distance(from,to)<=distance(from_dst,to_dst));
      nth_element(from_dst,from_dst+distance(from,to),to_dst);
      copy(from,to,from_dst);
    }
  };

  
  struct rtr_replacement {
    rtr_replacement(const field_set& fs,int ws) : window_size(ws),_fields(&fs) { }

    template<typename NewInst,typename Dst>
    void operator()(NewInst from,NewInst to,Dst from_dst,Dst to_dst) const {
      assert(window_size<=distance(from_dst,to_dst));

      for (;from!=to;++from)
	operator()(*from,from_dst,to_dst);
    }

    template<typename Dst,typename ScoreT>
    void operator()(const scored_instance<ScoreT>& inst,
		    Dst from_dst,Dst to_dst) const {
      util::lazy_random_selector select(distance(from_dst,to_dst));
      Dst closest=from_dst+select();
      int closest_distance=_fields->hamming_distance(inst,*closest);
      for (int i=1;i<window_size;++i) {
	Dst candidate=from_dst+select();
	int distance=_fields->hamming_distance(inst,*candidate);
	if (distance<closest_distance) {
	  closest_distance=distance;
	  closest=candidate;
	}
      }
      if (*closest<inst)
	*closest=inst;
    }

    int window_size;
  protected:
    const field_set* _fields;
  };
  
} //~namespace eda

#endif
