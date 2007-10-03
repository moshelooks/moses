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

#ifndef ANT_REWRITE_H
#define ANT_REWRITE_H

#include "rewrite.h"

namespace rewrite {
  template<typename T>
  class AntSimplify : public RecRule<T> {
  public:
    typedef typename Rule<T>::sib_it sib_it;
    void visit(tree<T>& tr,sib_it it) const {
      if (is_if_food(*it)) {
	//first insert progns underneath it
	if (!is_progn(*it.begin()))
	  tr.insert_above(it.begin(),create_progn<T>());
	if (!is_progn(*tr.last_child(it)))
	  tr.insert_above(tr.last_child(it),create_progn<T>());
	
	if (it.begin().number_of_children()>0 && 
	    is_if_food(*it.begin().begin()))
          liftLeft(tr,it.begin().begin());
        if (tr.last_child(it).number_of_children()>0 &&
	    is_if_food(*tr.last_child(it).begin()))
	  liftRight(tr,tr.last_child(it).begin());
	if (tr.equal_subtree(it.begin(),tr.last_child(it)))
	  liftLeft(tr,it);
      } else if (is_progn(*it) && it.number_of_children()>1) { //turn, turn
	for (sib_it sib=it.begin(),nxt=++it.begin();nxt!=it.end();sib=nxt++) {
	  if (is_turn(*sib) && is_turn(*nxt)) {
	    if ((is_right(*sib) && is_left(*nxt)) ||
		(is_left(*sib) && is_right(*nxt)) ||
		(is_reversal(*sib) && is_reversal(*nxt))) {
	      tr.erase(sib);
	      nxt=tr.erase(nxt);
	      if (nxt==it.end())
		break;
	    } else if ((is_right(*sib) && is_right(*nxt)) ||
		       (is_left(*sib) && is_left(*nxt))) {
	      tr.erase(sib);
	      *nxt=create_reversal<T>();
	    } else if ((is_reversal(*sib) && is_left(*nxt)) || 
		       (is_left(*sib) && is_reversal(*nxt))) {
	      tr.erase(sib);
	      *nxt=create_right<T>();
	    } else if ((is_reversal(*sib) && is_right(*nxt)) || 
		       (is_right(*sib) && is_reversal(*nxt))) {
	      tr.erase(sib);
	      *nxt=create_left<T>();
	    }
	  }
	}
      }
    }

    void liftLeft(tree<T>& tr,sib_it it) const {
      tr.reparent(it,it.begin());
      *it=*it.begin();
      tr.erase(it.begin());
      tr.erase(it.begin());
    }

    void liftRight(tree<T>& tr,sib_it it) const {
      tr.erase(it.begin());
      tr.reparent(it,it.begin());
      *it=*it.begin();
      tr.erase(it.begin());
    }
  };
  template<typename T>
  void ant_simplify(tree<T>& t) { AntSimplify<T>()(t); }

} //~namespace rewrite

#endif
