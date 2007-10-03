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

#ifndef _MOSES_TYPES_H
#define _MOSES_TYPES_H

#include "util/functional.h"
#include "combo/vertex.h"
#include "combo/complexity.h"
#include "util/foreach.h"

namespace moses {
  
  //basic types
  typedef double score_t;
  typedef combo::complexity_t complexity_t;

  typedef std::pair<score_t,complexity_t> tree_score;
  typedef util::tagged_item<combo::vtree,tree_score> scored_tree;

  typedef std::vector<float> behavioral_score;

  typedef util::tagged_item<behavioral_score,tree_score> behavioral_tree_score;
  typedef util::tagged_item<combo::vtree,
			    behavioral_tree_score> behavioral_scored_tree;
  
  extern const tree_score worst_possible_score;

  //convenience accessors
  inline const combo::vtree& get_tree(const scored_tree& st) { 
    return st.first; 
  }
  inline const combo::vtree& get_tree(const behavioral_scored_tree& bst) { 
    return bst.first;
  }

  inline complexity_t get_complexity(const tree_score& ts) { 
    return ts.second; 
  }
  inline complexity_t get_complexity(const behavioral_tree_score& ts) { 
    return get_complexity(ts.second);
  }
  inline complexity_t get_complexity(const behavioral_scored_tree& bst) { 
    return get_complexity(bst.second);
  }
  inline complexity_t get_complexity(const scored_tree& st) { 
    return get_complexity(st.second);
  }

  inline score_t get_score(const tree_score& ts) { 
    return ts.first;
  }
  inline score_t get_score(const behavioral_tree_score& ts) { 
    return get_score(ts.second);
  }
  inline score_t get_score(const behavioral_scored_tree& bst) { 
    return get_score(bst.second);
  }
  inline score_t get_score(const scored_tree& st) { 
    return get_score(st.second);
  }

}

inline std::ostream& operator<<(std::ostream& out,const moses::tree_score& ts) {
  return (out << "[score=" << ts.first << ", complexity=" << -ts.second << "]");
}
inline std::ostream& operator<<(std::ostream& out,
				const moses::behavioral_tree_score& s) {
  out << "[ ";
  foreach (float f,s.first)
    out << f << " ";
  out << "], " << s.second;
  return out;
}

#endif
